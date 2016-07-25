g++ -o daemon -g -O0 daemon.cpp lib/json_reader.cpp lib/json_value.cpp lib/json_writer.cpp -I./include -I../out/include -L../out/lib -lbitmailcore -lcurl -lssl -lcrypto -lws2_32 -lgdi32
