#!/bin/bash
# launch.sh [hours] -- start the measuring host on AWS: bare metal as close to
# HttpArena's Threadripper PRO 3995WX (Zen 2, one socket, 64 cores) as AWS
# has -- c6a.metal (Zen 3, two sockets of 48 cores, 192 threads), Ubuntu
# 24.04 like the board's host.
#
# It COSTS while it runs (about 8 USD an hour), so it ends itself: the host
# shuts down after `hours` (default 8), and shutting down terminates it.
# finish.sh ends it sooner. Writes the instance id and address to
# ~/.scaly-bench for the other scripts.
#
#   AWS_REGION   default eu-central-1;  TYPE   default c6a.metal
set -eu
HOURS="${1:-8}"
export AWS_DEFAULT_REGION="${AWS_REGION:-eu-central-1}"
TYPE="${TYPE:-c6a.metal}"
KEY=scaly-bench
PEM="$HOME/.ssh/scaly-bench.pem"
SG=scaly-bench
STATE="$HOME/.scaly-bench"

AMI=$(aws ec2 describe-images --owners 099720109477 \
  --filters "Name=name,Values=ubuntu/images/hvm-ssd-gp3/ubuntu-noble-24.04-amd64-server-*" \
  --query "sort_by(Images,&CreationDate)[-1].ImageId" --output text)

if ! aws ec2 describe-key-pairs --key-names "$KEY" > /dev/null 2>&1; then
  aws ec2 create-key-pair --key-name "$KEY" --key-type ed25519 --query KeyMaterial --output text > "$PEM"
  chmod 600 "$PEM"
fi
[ -f "$PEM" ] || { echo "launch.sh: the key pair $KEY exists, but not $PEM"; exit 1; }

SGID=$(aws ec2 describe-security-groups --group-names "$SG" --query 'SecurityGroups[0].GroupId' --output text 2> /dev/null) \
  || SGID=$(aws ec2 create-security-group --group-name "$SG" \
       --description "scaly benchmark host: ssh from one address" --query GroupId --output text)
MYIP=$(curl -fsS https://checkip.amazonaws.com)
aws ec2 authorize-security-group-ingress --group-id "$SGID" --protocol tcp --port 22 \
  --cidr "$MYIP/32" > /dev/null 2>&1 || true

USERDATA="$(mktemp)"
trap 'rm -f "$USERDATA"' EXIT
printf '#!/bin/bash\nshutdown -h +%s\n' "$((HOURS * 60))" > "$USERDATA"

ID=$(aws ec2 run-instances --instance-type "$TYPE" --image-id "$AMI" \
  --key-name "$KEY" --security-group-ids "$SGID" \
  --instance-initiated-shutdown-behavior terminate \
  --block-device-mappings 'DeviceName=/dev/sda1,Ebs={VolumeSize=300,VolumeType=gp3,DeleteOnTermination=true}' \
  --user-data "file://$USERDATA" \
  --tag-specifications 'ResourceType=instance,Tags=[{Key=Name,Value=scaly-bench}]' \
  --query 'Instances[0].InstanceId' --output text)
echo "launch.sh: $ID ($TYPE, $AMI), ends itself after $HOURS h"
aws ec2 wait instance-running --instance-ids "$ID"
HOST=$(aws ec2 describe-instances --instance-ids "$ID" \
  --query 'Reservations[0].Instances[0].PublicDnsName' --output text)
printf 'ID=%s\nHOST=%s\nPEM=%s\nREGION=%s\n' "$ID" "$HOST" "$PEM" "$AWS_DEFAULT_REGION" > "$STATE"
echo "launch.sh: running at $HOST -- bare metal takes some minutes more before ssh answers"
until ssh -i "$PEM" -o StrictHostKeyChecking=accept-new -o ConnectTimeout=10 "ubuntu@$HOST" true 2> /dev/null; do
  sleep 20
done
echo "launch.sh: ssh answers"
