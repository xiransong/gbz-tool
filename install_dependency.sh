mkdir -p dependency
cd dependency

DEP_DIR=$(pwd)

git clone https://github.com/vgteam/sdsl-lite.git
cd sdsl-lite
./install.sh $DEP_DIR

cd $DEP_DIR
git clone https://github.com/vgteam/libhandlegraph.git
cd libhandlegraph
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$DEP_DIR
make
make install

cd $DEP_DIR
git clone https://github.com/jltsiren/gbwt.git
cd gbwt
./install.sh $DEP_DIR

cd $DEP_DIR
git clone https://github.com/jltsiren/gbwtgraph.git
cd gbwtgraph
./install.sh $DEP_DIR
