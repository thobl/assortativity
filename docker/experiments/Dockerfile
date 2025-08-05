FROM python:3.13.5-slim

########################################
## install required dependencies

RUN apt-get update

# dependencies for building the c++ code
RUN apt-get install -y git g++ cmake make

# python dependencies
RUN pip install run-experiments==1.0.8

# dependencies for downloading konect data
RUN apt-get install -y wget bzip2 zip


########################################
## some quality of life improvements

## create user `docker`
RUN useradd -s /bin/bash -m docker
## start with "random" user/group id (not really necessary)
RUN usermod -u 55555 docker
RUN groupmod -g 55555 docker

## on root login, run `set_user_group.sh` script: changes user/group
## of `docker` user to mimic the user owning experiments.py on the
## host and then logs in as `docker` user
COPY docker/set_user_group.sh /root/set_user_group.sh
RUN chmod +x /root/set_user_group.sh
RUN echo "/root/set_user_group.sh; exit" >> /root/.bashrc

## welcome message + colored prompt
RUN echo "PS1='\e[1m\e[32mdocker@assortativity:\e[33m\w\e[0m $ '" >> /home/docker/.bashrc
COPY docker/welcome_experiments.sh /home/docker/welcome.sh
RUN chmod +x /home/docker/welcome.sh
RUN echo "/home/docker/welcome.sh" >> /home/docker/.bashrc

