#!/bin/bash
docker build \
--file=tf.Dockerfile \
--tag="tf:0.1" \
.

# GUI visualization
# https://answers.ros.org/question/300113/docker-how-to-use-rviz-and-gazebo-from-a-container/
XAUTH=/tmp/.docker.xauth

echo "Preparing Xauthority data..."
xauth_list=$(xauth nlist :0 | tail -n 1 | sed -e 's/^..../ffff/')
if [ ! -f $XAUTH ]; then
    if [ ! -z "$xauth_list" ]; then
        echo $xauth_list | xauth -f $XAUTH nmerge -
    else
        touch $XAUTH
    fi
    chmod a+r $XAUTH
fi

echo "Done."
echo ""
echo "Verifying file contents:"
file $XAUTH
echo "--> It should say \"X11 Xauthority data\"."
echo ""
echo "Permissions:"
ls -FAlh $XAUTH
echo ""
echo "Running docker..."

# Create a container (to be destroyed upon exit)
docker run \
--interactive \
--tty \
--rm \
--gpus all \
--privileged \
--net=host \
-e "DISPLAY=$DISPLAY" \
-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH -e XAUTHORITY=$XAUTH \
--name=tf-container \
--mount type=bind,source="$(pwd)",target=/monodepth \
tf:0.1
