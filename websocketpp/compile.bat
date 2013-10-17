REM SET boost_path=..\..\..\boost_1_51_0\
REM SET websocketpp_path=..\..\..\websocket\websocketpp-master
SET boost_path=E:\boost\boost_1_51_0\
SET websocketpp_path=E:\progetti\websocketpp\websocketpp-master\

cl /EHa /DWIN32 /I%websocketpp_path% /I%boost_path% echo_server.cpp /link /LIBPATH:%boost_path%stage\lib
