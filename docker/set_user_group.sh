
FILE="/experiments/experiments.py"
USER=$(stat -c %u "$FILE")
GROUP=$(stat -c %g "$FILE")

echo "detected file owner on the host: $USER:$GROUP"
echo "changing UID:GID to $USER:$GROUP"

usermod -u $USER docker
groupmod -g $GROUP docker

su docker
