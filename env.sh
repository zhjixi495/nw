export TARGET=x86
#export TARGET=ARM

export PATH=/opt/nuc/tool/arm_linux_4.8/bin:$PATH
SOURCE_DIR=$PWD
RELEASE_DIR=$PWD/release
export  LD_LIBRARY_PATH=$RELEASE_DIR:$LD_LIBRARY_PATH


export SOURCE_DIR RELEASE_DIR LD_LIBRARY_PATH
echo $PATH
