FROM centos:7
MAINTAINER phrk

RUN yum install -y epel-release.noarch
RUN yum install -y openssl boost libpqxx-devel jansson protobuf
