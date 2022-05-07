# Notes for Generating Docker Image

### References
- [Introduction and a Hands-On Tutorial](https://mp.weixin.qq.com/s?__biz=MzAxODI5ODMwOA==&mid=2666542689&idx=1&sn=aa29805d628b6f41866a7977e88a235f&chksm=80dcf2cab7ab7bdc5d3a60463f9dba0d3f19af54d6a5da5d5f7bb1b737bf72903a4442accbaf&scene=21#wechat_redirect)
- [An Example Repo](https://github.com/orian/cppenv/blob/master/Dockerfile)

### Commands

Build the default image

```bash
docker image build --no-cache -t panen:default -f dockerfile_default .
docker tag panen:default weiminghu123/panen:default
docker push weiminghu123/panen:default
```

Build the torch image

```bash
docker image build --no-cache -t panen:torch -f dockerfile_torch .
docker tag panen:torch weiminghu123/panen:torch
docker push weiminghu123/panen:torch
```

Other commands with comments

```bash
# Build docker image
docker image build -t panen .

# Build docerk image with tag
docker image build --no-cache -t panen:default -f dockerfile_default .

# List available docker images
docker image ls

# Run docker image
docker container run -i -t panen:default

# Run docker image and remove the image at exit
docker container run -i -t --rm panen

# Assign a tag in the repository to the image
docker tag panen:default weiminghu123/panen:default

# Publish changes to docker hub
docker push weiminghu123/panen:default
```
