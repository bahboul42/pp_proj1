javac *.java
MAIN=`grep -l main *.java`
java -ea ${MAIN%.java}

