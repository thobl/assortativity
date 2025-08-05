FROM r-base:4.5.1

########################################
## install required dependencies

RUN R -e "install.packages('renv', repos = c(CRAN = 'https://cloud.r-project.org'))"

## general dependencies
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y libfreetype6-dev

## R libraries
WORKDIR /tmp
COPY R/renv.lock renv.lock
RUN R -e "renv::restore()"

## latex font + symlink so R finds it
RUN apt-get install -y fonts-lmodern
RUN ln -s "/usr/share/texmf/fonts/opentype"
RUN ln -s "/usr/share/texmf/fonts/opentype/" "/usr/local/share/fonts/opentype"


########################################
## some quality of life improvements

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
RUN echo "PS1='\e[1m\e[32mdocker@assortativity-eval:\e[33m\w\e[0m $ '" >> /home/docker/.bashrc
COPY docker/welcome_eval.sh /home/docker/welcome.sh
RUN chmod +x /home/docker/welcome.sh
RUN echo "/home/docker/welcome.sh" >> /home/docker/.bashrc

