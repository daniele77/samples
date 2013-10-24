REM SET boost_path=\Users\Daniele\Documents\software\boost_1_54_0\
SET boost_path=E:\boost\boost_1_54_0\

cl /EHa /I%boost_path% basic.cpp /Febasic /link /LIBPATH:%boost_path%stage\lib

