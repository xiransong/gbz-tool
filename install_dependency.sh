mkdir -p dependency
cd dependency

DEP_DIR=$(pwd)

# Install sdsl-lite
git clone https://github.com/vgteam/sdsl-lite.git
cd sdsl-lite
./install.sh $DEP_DIR
cd $DEP_DIR

# Install libhandlegraph
git clone https://github.com/vgteam/libhandlegraph.git
cd libhandlegraph
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$DEP_DIR
make
make install
cd $DEP_DIR

# Install gbwt
git clone https://github.com/jltsiren/gbwt.git
cd gbwt
./install.sh $DEP_DIR
cd $DEP_DIR

# Install gbwtgraph
git clone https://github.com/jltsiren/gbwtgraph.git
cd gbwtgraph
./install.sh $DEP_DIR
cd $DEP_DIR

# Install nlohmann/json
git clone https://github.com/nlohmann/json.git
cd json
mkdir -p $DEP_DIR/include/nlohmann
cp -r single_include/nlohmann/* $DEP_DIR/include/nlohmann/
cd $DEP_DIR