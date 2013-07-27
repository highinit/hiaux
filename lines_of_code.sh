find . -name "*.h" -not -path "./thirdparty/*" -o -name "*.cpp" -not -path "./thirdparty/*" | xargs wc -l
