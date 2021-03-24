CC = g++ -g -Wall -fPIC
AR = ar -r
PRT = protoc -I=./ --cpp_out=./

.PHONY: clearObj  clearOut clearLog

protoFile/%.pb.cc: protoFile/%.proto
	$(PRT) $^

protoFile/%.pb.h: protoFile/%.proto
	$(PRT) $^

protoFile/%.o: protoFile/%.pb.cc protoFile/%.pb.h
	$(CC) -c $< -o $@

common/%.o: common/%.cpp common/%.h
	$(CC) -c $< -o $@

server/%.o: server/%.cpp server/%.h
	$(CC) -c $< -o $@

client/%.o: client/%.cpp client/%.h
	$(CC) -c $< -o $@

client/client.out: protoFile/protoMsg.pb.o client/cursesLib.o common/commonUtils.o client/client.o common/socketLib.o client/msgLib.o client/clientUtils.o
	$(CC) -o $@ $^ /usr/local/lib/libprotobuf.a -lcurses

server/server.out: protoFile/protoMsg.pb.o server/gameControl.o server/gameProcess.o server/server.o common/socketLib.o common/commonUtils.o server/serverUtils.o
	$(CC) -o $@ $^ /usr/local/lib/libprotobuf.a

clearObj:
	rm protoFile/*.o client/*.o server/*.o common/*.o

clearOut:
	rm server/*.out client/*.out