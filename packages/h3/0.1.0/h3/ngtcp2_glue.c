/* ngtcp2 for the h3 package (ROADMAP-http.md, stage G): what Scaly cannot
 * write for itself. ngtcp2's settings, transport parameters and callback
 * table are open C structs to fill -- the parameters hold a struct
 * sockaddr_in and a sockaddr_in6, the OS's layouts (shim rule (a)) -- and
 * several of its entry points are macros over versioned symbols. Every
 * function here takes and answers pointers and integers only; the
 * connection table, the streams, HTTP/3, QPACK and the timers are Scaly
 * (h3/Server.scaly), and so are the callbacks that carry data. */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ngtcp2/ngtcp2.h>
#include <ngtcp2/ngtcp2_crypto.h>
#include <ngtcp2/ngtcp2_crypto_ossl.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>

/* The secret stateless reset tokens are derived from: one per process. */
static uint8_t scaly_quic_secret[32];
static ngtcp2_callbacks scaly_quic_cbs;
/* The Scaly side's say in a new connection ID: it writes its routing bytes
 * into the random ID (slot and generation of the connection), 0 when well. */
typedef int (*scaly_quic_cid_cb)(void* user_data, uint8_t* cid, size_t cidlen);
static scaly_quic_cid_cb scaly_quic_on_new_cid;

static void scaly_quic_rand(uint8_t* dest, size_t destlen, const ngtcp2_rand_ctx* ctx)
{
    (void)ctx;
    if (RAND_bytes(dest, (int)destlen) != 1)
        abort();
}

static int scaly_quic_new_cid(ngtcp2_conn* conn, ngtcp2_cid* cid,
                              ngtcp2_stateless_reset_token* token, size_t cidlen,
                              void* user_data)
{
    (void)conn;
    if (RAND_bytes(cid->data, (int)cidlen) != 1)
        return NGTCP2_ERR_CALLBACK_FAILURE;
    cid->datalen = cidlen;
    if (scaly_quic_on_new_cid(user_data, cid->data, cidlen) != 0)
        return NGTCP2_ERR_CALLBACK_FAILURE;
    if (ngtcp2_crypto_generate_stateless_reset_token(token->data, scaly_quic_secret,
                                                     sizeof scaly_quic_secret, cid) != 0)
        return NGTCP2_ERR_CALLBACK_FAILURE;
    return 0;
}

/* Once per process, before any connection: the TLS backend, the secret and
 * the callback table -- the crypto helpers' own callbacks and the Scaly
 * procedures named here (each with ngtcp2's signature for that slot). 0
 * when well. */
int scaly_quic_setup(void* recv_stream_data, void* acked_stream_data_offset,
                     void* stream_open, void* stream_close,
                     void* extend_max_remote_streams_bidi, void* handshake_completed,
                     void* on_new_cid)
{
    if (ngtcp2_crypto_ossl_init() != 0)
        return -1;
    if (RAND_bytes(scaly_quic_secret, sizeof scaly_quic_secret) != 1)
        return -1;
    memset(&scaly_quic_cbs, 0, sizeof scaly_quic_cbs);
    scaly_quic_cbs.recv_client_initial = ngtcp2_crypto_recv_client_initial_cb;
    scaly_quic_cbs.recv_crypto_data = ngtcp2_crypto_recv_crypto_data_cb;
    scaly_quic_cbs.encrypt = ngtcp2_crypto_encrypt_cb;
    scaly_quic_cbs.decrypt = ngtcp2_crypto_decrypt_cb;
    scaly_quic_cbs.hp_mask = ngtcp2_crypto_hp_mask_cb;
    scaly_quic_cbs.update_key = ngtcp2_crypto_update_key_cb;
    scaly_quic_cbs.delete_crypto_aead_ctx = ngtcp2_crypto_delete_crypto_aead_ctx_cb;
    scaly_quic_cbs.delete_crypto_cipher_ctx = ngtcp2_crypto_delete_crypto_cipher_ctx_cb;
    scaly_quic_cbs.get_path_challenge_data2 = ngtcp2_crypto_get_path_challenge_data2_cb;
    scaly_quic_cbs.version_negotiation = ngtcp2_crypto_version_negotiation_cb;
    scaly_quic_cbs.rand = scaly_quic_rand;
    scaly_quic_cbs.get_new_connection_id2 = scaly_quic_new_cid;
    scaly_quic_cbs.recv_stream_data = (ngtcp2_recv_stream_data)recv_stream_data;
    scaly_quic_cbs.acked_stream_data_offset = (ngtcp2_acked_stream_data_offset)acked_stream_data_offset;
    scaly_quic_cbs.stream_open = (ngtcp2_stream_open)stream_open;
    scaly_quic_cbs.stream_close = (ngtcp2_stream_close)stream_close;
    scaly_quic_cbs.extend_max_remote_streams_bidi = (ngtcp2_extend_max_streams)extend_max_remote_streams_bidi;
    scaly_quic_cbs.handshake_completed = (ngtcp2_handshake_completed)handshake_completed;
    scaly_quic_on_new_cid = (scaly_quic_cid_cb)on_new_cid;
    return 0;
}

/* A server connection for a client's first packet: dcid and scid as the
 * client sent them (its Initial's), our own ID, the path as byte blobs of
 * struct sockaddr, the version, the clock, and the limits it grants. 0 and
 * the connection through pconn, or ngtcp2's negative error. */
int scaly_quic_conn_new(void** pconn, const uint8_t* client_dcid, size_t client_dcidlen,
                        const uint8_t* client_scid, size_t client_scidlen,
                        const uint8_t* own_cid, size_t own_cidlen,
                        const void* local, size_t locallen, const void* remote, size_t remotelen,
                        uint32_t version, uint64_t now, uint64_t max_data, uint64_t max_stream_data,
                        uint64_t max_streams_bidi, uint64_t idle_timeout_ms, void* user_data)
{
    ngtcp2_cid dcid, scid, ocid;
    ngtcp2_settings settings;
    ngtcp2_transport_params params;
    ngtcp2_path path;
    ngtcp2_conn* conn = NULL;
    int rv;
    ngtcp2_cid_init(&dcid, client_scid, client_scidlen);
    ngtcp2_cid_init(&ocid, client_dcid, client_dcidlen);
    ngtcp2_cid_init(&scid, own_cid, own_cidlen);
    ngtcp2_settings_default(&settings);
    settings.initial_ts = now;
    ngtcp2_transport_params_default(&params);
    params.initial_max_stream_data_bidi_local = max_stream_data;
    params.initial_max_stream_data_bidi_remote = max_stream_data;
    params.initial_max_stream_data_uni = max_stream_data;
    params.initial_max_data = max_data;
    params.initial_max_streams_bidi = max_streams_bidi;
    params.initial_max_streams_uni = 8;
    params.max_idle_timeout = idle_timeout_ms * NGTCP2_MILLISECONDS;
    params.active_connection_id_limit = 7;
    params.grease_quic_bit = 1;
    params.original_dcid = ocid;
    params.original_dcid_present = 1;
    params.stateless_reset_token_present = 1;
    if (ngtcp2_crypto_generate_stateless_reset_token(params.stateless_reset_token, scaly_quic_secret,
                                                     sizeof scaly_quic_secret, &scid) != 0)
        return NGTCP2_ERR_CALLBACK_FAILURE;
    path.local.addr = (ngtcp2_sockaddr*)local;
    path.local.addrlen = (ngtcp2_socklen)locallen;
    path.remote.addr = (ngtcp2_sockaddr*)remote;
    path.remote.addrlen = (ngtcp2_socklen)remotelen;
    path.user_data = NULL;
    rv = ngtcp2_conn_server_new(&conn, &dcid, &scid, &path, version, &scaly_quic_cbs,
                                &settings, &params, NULL, user_data);
    if (rv != 0)
        return rv;
    *pconn = conn;
    return 0;
}

static ngtcp2_conn* scaly_quic_get_conn(ngtcp2_crypto_conn_ref* ref)
{
    return (ngtcp2_conn*)ref->user_data;
}

/* The TLS session of a connection: an SSL of ctx that ngtcp2 drives
 * through OpenSSL's QUIC TLS interface, its app data the reference ngtcp2's
 * crypto helpers find the connection by. 0 and both handles, or -1. */
int scaly_quic_tls_new(void* ssl_ctx, void* conn, void** pssl, void** poctx)
{
    ngtcp2_crypto_ossl_ctx* octx = NULL;
    ngtcp2_crypto_conn_ref* ref;
    SSL* ssl = SSL_new((SSL_CTX*)ssl_ctx);
    if (ssl == NULL)
        return -1;
    if (ngtcp2_crypto_ossl_ctx_new(&octx, NULL) != 0)
    {
        SSL_free(ssl);
        return -1;
    }
    ngtcp2_crypto_ossl_ctx_set_ssl(octx, ssl);
    ref = (ngtcp2_crypto_conn_ref*)malloc(sizeof *ref);
    if (ref == NULL || ngtcp2_crypto_ossl_configure_server_session(ssl) != 0)
    {
        free(ref);
        ngtcp2_crypto_ossl_ctx_del(octx);
        SSL_free(ssl);
        return -1;
    }
    ref->get_conn = scaly_quic_get_conn;
    ref->user_data = conn;
    SSL_set_app_data(ssl, ref);
    SSL_set_accept_state(ssl);
    ngtcp2_conn_set_tls_native_handle((ngtcp2_conn*)conn, octx);
    *pssl = ssl;
    *poctx = octx;
    return 0;
}

/* The TLS session gone, before its connection is (the app data first, as
 * the crypto helpers ask). */
void scaly_quic_tls_free(void* ssl, void* octx)
{
    ngtcp2_crypto_conn_ref* ref = (ngtcp2_crypto_conn_ref*)SSL_get_app_data((SSL*)ssl);
    SSL_set_app_data((SSL*)ssl, NULL);
    free(ref);
    ngtcp2_crypto_ossl_ctx_del((ngtcp2_crypto_ossl_ctx*)octx);
    SSL_free((SSL*)ssl);
}

/* A datagram's version and destination connection ID (short_dcidlen for a
 * short header, which does not say): 0, or ngtcp2's negative error --
 * NGTCP2_ERR_VERSION_NEGOTIATION for a version it does not speak. */
int scaly_quic_decode(const uint8_t* pkt, size_t len, size_t short_dcidlen,
                      uint32_t* version, const uint8_t** dcid, size_t* dcidlen)
{
    ngtcp2_version_cid vc;
    int rv = ngtcp2_pkt_decode_version_cid(&vc, pkt, len, short_dcidlen);
    if (rv != 0)
        return rv;
    *version = vc.version;
    *dcid = vc.dcid;
    *dcidlen = vc.dcidlen;
    return 0;
}

/* Does a datagram open a connection? 0 and its version and both IDs (room
 * for NGTCP2_MAX_CIDLEN each), or a negative answer: drop it. */
int scaly_quic_accept(const uint8_t* pkt, size_t len, uint32_t* version,
                      uint8_t* dcid, size_t* dcidlen, uint8_t* scid, size_t* scidlen)
{
    ngtcp2_pkt_hd hd;
    int rv = ngtcp2_accept(&hd, pkt, len);
    if (rv != 0)
        return rv;
    *version = hd.version;
    memcpy(dcid, hd.dcid.data, hd.dcid.datalen);
    *dcidlen = hd.dcid.datalen;
    memcpy(scid, hd.scid.data, hd.scid.datalen);
    *scidlen = hd.scid.datalen;
    return 0;
}

/* One datagram for conn, from remote to local (struct sockaddr blobs). */
int scaly_quic_read(void* conn, const void* local, size_t locallen, const void* remote,
                    size_t remotelen, const uint8_t* data, size_t len, uint64_t now)
{
    ngtcp2_path path;
    ngtcp2_pkt_info pi;
    path.local.addr = (ngtcp2_sockaddr*)local;
    path.local.addrlen = (ngtcp2_socklen)locallen;
    path.remote.addr = (ngtcp2_sockaddr*)remote;
    path.remote.addrlen = (ngtcp2_socklen)remotelen;
    path.user_data = NULL;
    memset(&pi, 0, sizeof pi);
    return ngtcp2_conn_read_pkt((ngtcp2_conn*)conn, &path, &pi, data, len, now);
}

/* The next packet of conn into dest, with data of stream_id (-1: none) as
 * far as it takes it (the count through pdatalen, -1 for none): the
 * packet's length, 0 for nothing to send, or ngtcp2's negative answer
 * (NGTCP2_ERR_WRITE_MORE: the packet has room for more). */
long long scaly_quic_write(void* conn, uint8_t* dest, size_t destlen, long long* pdatalen,
                           uint32_t flags, int64_t stream_id, const uint8_t* data, size_t datalen,
                           uint64_t now)
{
    ngtcp2_path_storage ps;
    ngtcp2_pkt_info pi;
    ngtcp2_vec v;
    ngtcp2_ssize consumed = -1;
    ngtcp2_ssize n;
    ngtcp2_path_storage_zero(&ps);
    v.base = (uint8_t*)data;
    v.len = datalen;
    n = ngtcp2_conn_writev_stream((ngtcp2_conn*)conn, &ps.path, &pi, dest, destlen, &consumed,
                                  flags, stream_id, datalen > 0 ? &v : NULL, datalen > 0 ? 1 : 0,
                                  now);
    *pdatalen = consumed;
    return n;
}

/* The CONNECTION_CLOSE packet ending conn (no error) into dest: its length,
 * or a negative answer. */
long long scaly_quic_close(void* conn, uint8_t* dest, size_t destlen, uint64_t now)
{
    ngtcp2_path_storage ps;
    ngtcp2_pkt_info pi;
    ngtcp2_ccerr ccerr;
    ngtcp2_path_storage_zero(&ps);
    ngtcp2_ccerr_default(&ccerr);
    return ngtcp2_conn_write_connection_close((ngtcp2_conn*)conn, &ps.path, &pi, dest, destlen,
                                              &ccerr, now);
}

/* ---- a client, for the h3 suite (tests/h3) ---- */

static ngtcp2_callbacks scaly_quic_client_cbs;

static int scaly_quic_client_new_cid(ngtcp2_conn* conn, ngtcp2_cid* cid,
                                     ngtcp2_stateless_reset_token* token, size_t cidlen,
                                     void* user_data)
{
    (void)conn;
    (void)user_data;
    if (RAND_bytes(cid->data, (int)cidlen) != 1 || RAND_bytes(token->data, sizeof token->data) != 1)
        return NGTCP2_ERR_CALLBACK_FAILURE;
    cid->datalen = cidlen;
    return 0;
}

/* A client connection from local to remote (struct sockaddr blobs), its
 * data callbacks Scaly procedures as in scaly_quic_setup: 0 and the
 * connection through pconn, or ngtcp2's negative error. */
int scaly_quic_client_new(void** pconn, const void* local, size_t locallen, const void* remote,
                          size_t remotelen, uint64_t now, void* recv_stream_data,
                          void* stream_close, void* handshake_completed, void* user_data)
{
    ngtcp2_cid dcid, scid;
    ngtcp2_settings settings;
    ngtcp2_transport_params params;
    ngtcp2_path path;
    ngtcp2_conn* conn = NULL;
    int rv;
    if (ngtcp2_crypto_ossl_init() != 0)
        return -1;
    memset(&scaly_quic_client_cbs, 0, sizeof scaly_quic_client_cbs);
    scaly_quic_client_cbs.client_initial = ngtcp2_crypto_client_initial_cb;
    scaly_quic_client_cbs.recv_crypto_data = ngtcp2_crypto_recv_crypto_data_cb;
    scaly_quic_client_cbs.encrypt = ngtcp2_crypto_encrypt_cb;
    scaly_quic_client_cbs.decrypt = ngtcp2_crypto_decrypt_cb;
    scaly_quic_client_cbs.hp_mask = ngtcp2_crypto_hp_mask_cb;
    scaly_quic_client_cbs.recv_retry = ngtcp2_crypto_recv_retry_cb;
    scaly_quic_client_cbs.update_key = ngtcp2_crypto_update_key_cb;
    scaly_quic_client_cbs.delete_crypto_aead_ctx = ngtcp2_crypto_delete_crypto_aead_ctx_cb;
    scaly_quic_client_cbs.delete_crypto_cipher_ctx = ngtcp2_crypto_delete_crypto_cipher_ctx_cb;
    scaly_quic_client_cbs.get_path_challenge_data2 = ngtcp2_crypto_get_path_challenge_data2_cb;
    scaly_quic_client_cbs.version_negotiation = ngtcp2_crypto_version_negotiation_cb;
    scaly_quic_client_cbs.rand = scaly_quic_rand;
    scaly_quic_client_cbs.get_new_connection_id2 = scaly_quic_client_new_cid;
    scaly_quic_client_cbs.recv_stream_data = (ngtcp2_recv_stream_data)recv_stream_data;
    scaly_quic_client_cbs.stream_close = (ngtcp2_stream_close)stream_close;
    scaly_quic_client_cbs.handshake_completed = (ngtcp2_handshake_completed)handshake_completed;
    dcid.datalen = 18;
    scid.datalen = 8;
    if (RAND_bytes(dcid.data, (int)dcid.datalen) != 1 || RAND_bytes(scid.data, (int)scid.datalen) != 1)
        return -1;
    ngtcp2_settings_default(&settings);
    settings.initial_ts = now;
    ngtcp2_transport_params_default(&params);
    params.initial_max_streams_uni = 3;
    params.initial_max_stream_data_bidi_local = 1 << 20;
    params.initial_max_stream_data_uni = 1 << 20;
    params.initial_max_data = 16 << 20;
    path.local.addr = (ngtcp2_sockaddr*)local;
    path.local.addrlen = (ngtcp2_socklen)locallen;
    path.remote.addr = (ngtcp2_sockaddr*)remote;
    path.remote.addrlen = (ngtcp2_socklen)remotelen;
    path.user_data = NULL;
    rv = ngtcp2_conn_client_new(&conn, &dcid, &scid, &path, NGTCP2_PROTO_VER_V1,
                                &scaly_quic_client_cbs, &settings, &params, NULL, user_data);
    if (rv != 0)
        return rv;
    *pconn = conn;
    return 0;
}

/* The client's TLS session (ALPN h3): 0 and both handles, or -1. */
int scaly_quic_client_tls_new(void* ssl_ctx, void* conn, void** pssl, void** poctx)
{
    static const unsigned char alpn[] = {2, 'h', '3'};
    ngtcp2_crypto_ossl_ctx* octx = NULL;
    ngtcp2_crypto_conn_ref* ref;
    SSL* ssl = SSL_new((SSL_CTX*)ssl_ctx);
    if (ssl == NULL)
        return -1;
    if (ngtcp2_crypto_ossl_ctx_new(&octx, NULL) != 0)
    {
        SSL_free(ssl);
        return -1;
    }
    ngtcp2_crypto_ossl_ctx_set_ssl(octx, ssl);
    ref = (ngtcp2_crypto_conn_ref*)malloc(sizeof *ref);
    if (ref == NULL || ngtcp2_crypto_ossl_configure_client_session(ssl) != 0)
    {
        free(ref);
        ngtcp2_crypto_ossl_ctx_del(octx);
        SSL_free(ssl);
        return -1;
    }
    ref->get_conn = scaly_quic_get_conn;
    ref->user_data = conn;
    SSL_set_app_data(ssl, ref);
    SSL_set_connect_state(ssl);
    SSL_set_alpn_protos(ssl, alpn, sizeof alpn);
    ngtcp2_conn_set_tls_native_handle((ngtcp2_conn*)conn, octx);
    *pssl = ssl;
    *poctx = octx;
    return 0;
}

