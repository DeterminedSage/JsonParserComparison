# JsonParserComparison
rapidjson vs simdjson vs jsoniopipe

To execute this file in your local machine , make sure to integrate the relevant files from simdjson parser (Link : https://github.com/simdjson/simdjson ) , rapidjson parser (Link : https://github.com/Tencent/rapidjson/ ) && jsoniopipe (Link : https://github.com/schveiguy/jsoniopipe ) in the same directory as the one containing evaluate.cpp . 

NOTE : YOU DONT HAVE TO COPY THE ENTIRE REPO , JUST COPY THE RELEVENT FILES IN THE LOCAL REQUIREMENT .

Once this is done , to run evaluate.cpp , 
In the bash terminal , run the following command :

c++ -o evaluate evaluate.cpp simdjson.cpp
./evaluate.exe



Similarly , to run evaluate.d ,
In the bash terminal , run the following command :

dmd evaluate.d -of=evaluate.exe
./evaluate.exe

