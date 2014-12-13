FROM centos:latest
MAINTAINER phrk

RUN yum install -y epel-release.noarch
RUN yum install -y openssl boost libpqxx-devel jansson protobuf
