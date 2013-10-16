SET boost_path=..\..\..\boost_1_51_0\
SET websocketpp_path=..\..\..\websocket\websocketpp-master

cl /EHa /DWIN32 /I%websocketpp_path% /I%boost_path% echo_server.cpp /link /LIBPATH:%boost_path%stage\lib
