# Notes for Generating Docker Image

### References
- [Introduction and a Hands-On Tutorial](https://mp.weixin.qq.com/s?__biz=MzAxODI5ODMwOA==&mid=2666542689&idx=1&sn=aa29805d628b6f41866a7977e88a235f&chksm=80dcf2cab7ab7bdc5d3a60463f9dba0d3f19af54d6a5da5d5f7bb1b737bf72903a4442accbaf&scene=21#wechat_redirect)
- [An Example Repo](https://github.com/orian/cppenv/blob/master/Dockerfile)

### Commands

To generate a new docker image:

```
cd AnalogsEnsemble
sudo docker image build -t pef .
``` 

After the image is ready, check if it shows up in the list:

```
sodu docker image ls
```

To use this docker image:

```
sudo docker container run --rm pef
```

You should see a change in the command line prompt which signifies that you have been successfully using the docker image.

The option `--rm` tells docker to kill remove the container file once you exit from the container. You can also remove the option, and do it manually:

```
# in a new terminal
sudo docker container ls -all
sudo docker container rm [containerID]
```
