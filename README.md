# JsonParserComparison
rapidjson vs simdjson vs jsoniopipe



To execute this file in your local machine , make sure to integrate the relevant files in the same directory as the one containing evaluate.cpp :




simdjson parser : https://github.com/simdjson/simdjson 

rapidjson parser : https://github.com/Tencent/rapidjson/ 

jsoniopipe : https://github.com/schveiguy/jsoniopipe 

(For jsoniopipe and running evaluate.d , you also might need to refer 

DMD[D lang compiler] : https://github.com/dlang/dmd/tree/master

PHOBOS[D standard library] : https://github.com/dlang/phobos)



 

Once this is done , to run evaluate.cpp , 
Use the following bash CLI command :

c++ -o evaluate evaluate.cpp simdjson.cpp


./evaluate.exe




Similarly , to run evaluate.d ,
Use the following bash CLI command :

dmd evaluate.d -of=evaluate.exe


./evaluate.exe

