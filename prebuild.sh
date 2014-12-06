cd network/HttpApi
protoc --cpp_out=. *.proto
cd BinClient
protoc --cpp_out=. *.proto