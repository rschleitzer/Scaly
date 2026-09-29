// The C/C++ side of tools/bench/json/run.sh: the method of bench.scaly on the
// same file. Five rounds of `iterations` parses into each library's DOM
// (freed at once), and five of as many writes of a tree parsed once; the best
// round counts. Throughput is the FILE's size per second in both directions.
//
//   simdjson  dom::parser, REUSED across parses as its documentation asks (it
//             keeps its tape and string buffers; every other parser here
//             allocates its tree fresh); written back with simdjson::minify
//   yyjson    yyjson_read / yyjson_doc_free, written with yyjson_write
//
//   cppbench <file> [iterations]

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include <simdjson.h>
#include <yyjson.h>

static const int ROUNDS = 5;

template <typename F> static double best(int iterations, F f) {
    double best = 1e300;
    for (int r = 0; r < ROUNDS; r++) {
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < iterations; i++) f();
        double dt = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
        if (dt < best) best = dt;
    }
    return best;
}

static long mbps(size_t bytes, int iterations, double secs) {
    return (long)((double)bytes * iterations / secs / 1e6);
}

static volatile size_t sink;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: cppbench <file> [iterations]\n");
        return 2;
    }
    int iterations = argc > 2 ? atoi(argv[2]) : 20;
    std::ifstream in(argv[1], std::ios::binary);
    std::stringstream ss;
    ss << in.rdbuf();
    std::string data = ss.str();
    size_t n = data.size();

    {
        simdjson::padded_string padded(data);
        simdjson::dom::parser parser;
        double p = best(iterations, [&] {
            simdjson::dom::element doc = parser.parse(padded);
            sink = sink + (doc.is_object() ? 1 : 2);
        });
        simdjson::dom::parser keep;
        simdjson::dom::element tree = keep.parse(padded);
        double w = best(iterations, [&] {
            std::string out = simdjson::minify(tree);
            sink = sink + out.size();
        });
        printf("simdjson   parse %ld MB/s  write %ld MB/s  (dom::parser reused)\n", mbps(n, iterations, p), mbps(n, iterations, w));
    }
    {
        double p = best(iterations, [&] {
            yyjson_doc* doc = yyjson_read(data.data(), n, 0);
            sink = sink + yyjson_doc_get_read_size(doc);
            yyjson_doc_free(doc);
        });
        yyjson_doc* tree = yyjson_read(data.data(), n, 0);
        double w = best(iterations, [&] {
            size_t len = 0;
            char* out = yyjson_write(tree, 0, &len);
            sink = sink + len;
            free(out);
        });
        yyjson_doc_free(tree);
        printf("yyjson     parse %ld MB/s  write %ld MB/s\n", mbps(n, iterations, p), mbps(n, iterations, w));
    }
    return 0;
}
