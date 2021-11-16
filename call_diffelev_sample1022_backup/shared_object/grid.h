#pragma once

#include "vector_struct.h"
#include "polyhedral_struct.h"

namespace grid {
  // グリッドの区切り方を設定する構造体
  struct grid_config
  {
    point2d_t grid_origin = { 0,0 }; // グリッドの基準位置
    size2d_t grid_width = { 1,1 }; // グリッドの幅
  };

  // グリッド位置を示す2次元インデックス（番号）
  typedef std::array<int, 2> grid_index_t;

  class grid_triangle_container;

  // グリッド区切りで点を保持するクラス
  // グリッドごとの間引きやグリッド内の点を集めるなどで使用する
  class grid_container
  {
  public:
    grid_container(const grid_config& config = grid_config());
    
    const grid_config& get_config() const;

  public:
    // 3次元座標値を一つ登録
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool add(const point3d_t& point);

    // 3次元座標値を複数個登録
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool add(const std::vector<point3d_t>& points);

  public:
    // 2次元位置からグリッドのインデックスを取得
    static grid_index_t pos2idx(const point2d_t& input_pos, const grid_config& grid_cfg);

    // 3次元位置からグリッドのインデックスを取得
    static grid_index_t pos2idx(const point3d_t& input_pos, const grid_config& grid_cfg);

    // インデックスからそのグリッドのXY方向の中心位置を取得
    static point2d_t idx2pos(const grid_index_t& index, const grid_config& grid_cfg);

  public:
    // 全グリッドが空かどうかを判定する
    bool empty() const;

    // 指定グリッド内にある点の個数を取得する
    size_t count(const grid_index_t& index) const;

    // 指定グリッド内の点を全て取得する
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect(const grid_index_t& index, std::vector<point3d_t>& collected_points) const;

    // 登録された点を全て取得する
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect_all(std::vector<point3d_t>& collected_points) const;

  public:
    // 変化点となる辺(辺の両側の面が1平面とならない辺)付近の点を取り除く
    // tria_containerに比較する三角形の集合体のデータ、thresholdに閾値が入る
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool remove_near_nonplaner_edge(const grid_triangle_container& tria_container, double threshold);

    // 面の法線方向に閾値以上離れた点を取り除く
    // tria_containerに比較する三角形の集合体のデータ、thresholdに閾値が入る
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool remove_far_triangles(const grid_triangle_container& tria_container, double threshold, size_t thread_num);

  public:
    // Zの最大値で間引き
    // 登録された点集合のうち、同一のグリッド内の点が1点以下になるように間引く
    // この際、同一グリッド内のうちZの値が最大のものが残るようにする
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool thin_max();

    //// Zの最小値で間引き
    // 登録された点集合のうち、同一のグリッド内の点が1点以下になるように間引く
    // この際、同一グリッド内のうちZの値が最小のものが残るようにする
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool thin_min();

    //// Zの中央値で間引き
    // 登録された点集合のうち、同一のグリッド内の点が1点以下になるように間引く
    // この際、同一グリッド内のうちZの値で並び変えた場合のちょうど真ん中のものが残るようにする
    bool thin_median();

    //// Zの最頻値で間引き
    // 登録された点集合のうち、同一のグリッド内の点が1点以下になるように間引く
    // この際、同一グリッド内のうちZの値でintervalの入力値の間隔で分類し、最も多い分類の値が残るようにする
    bool thin_mode(double interval);

  public:
    ~grid_container();
    
    grid_container(const grid_container&);
    grid_container(grid_container&&);
    grid_container& operator = (const grid_container&);
    grid_container& operator = (grid_container&&);

  private:
    class impl;
    impl* pimpl;
  };

  // グリッド区切りで三角形の集合体で形成された形状データを保持するクラス
  // グリッドごとの間引きやグリッド内の点を集めるなどで使用する
  class grid_triangle_container
  {
  public:
    grid_triangle_container(const grid_config& config = grid_config());

    const grid_config& get_config() const;

  public:
    // 3次元座標値を一つ登録
    // 処理に成功すれば登録された点のindexが返り、何かしらエラーが発生した場合は-1が返る
    int add_vertex(const point3d_t& point);

    // 3次元座標値を複数個登録
    // 処理に成功すれば登録された点の先頭indexが返り、何かしらエラーが発生した場合は-1と-1のペアが返る
    int add_verteces(const std::vector<point3d_t>& points);
    
    // 三角形(add_vertexで返ったindex3つで指定)を一つ登録
    // 処理に成功すれば登録された三角形のindexが返り、何かしらエラーが発生した場合は-1が返る
    int add_triangle(const tria_index_t& tria);

    // 三角形(add_vertexで返ったindex3つで指定)を複数個登録
    // 処理に成功すれば登録された三角形の先頭indexが返り、何かしらエラーが発生した場合は-1のペアが返る
    int add_triangles(const std::vector<tria_index_t>& trias, int shift);

    // 三角形の集合体を登録
    // 処理に成功すれば登録された点の先頭indexと登録された三角形の先頭indexのペアが返り、何かしらエラーが発生した場合は-1,-1のペアが返る
    std::pair<int, int> add_polyhedral(const polyhedral_tri3d_t& poly);

  public:
    size_t get_vertex_num() const;
    size_t get_triangle_num() const;
    point3d_t get_vertex(int index) const;
    tria_index_t get_triangle(int index) const;
    bool copy_to(polyhedral_tri3d_t& poly) const;

  public:
    // 指定グリッド内に関連する三角形の個数を取得する
    size_t count(const grid_index_t& index) const;

    // 指定グリッド内に関連する三角形を全て取得し、indexの列としてcollected_tria_indexesに加える
    // 入力のcollected_tria_indexesは昇順ソート済みである必要がある
    // その場合、結果のcollected_tria_indexesは昇順ソート済みのものとして得られる
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect(const grid_index_t& index, std::vector<int>& collected_tria_indexes) const;

    // 指定グリッド内に関連する三角形を全て取得し、indexの列としてcollected_tria_indexesに加える
    // posで指定した位置から半径radius内にある三角形(少しでもかすっていればOK)を全て取得する
    // posからradiusより遠い三角形も取ってくる可能性はあるが、radiusより近い三角形が除外されることはない
    // 入力のcollected_tria_indexesは昇順ソート済みである必要がある
    // その場合、結果のcollected_tria_indexesは昇順ソート済みのものとして得られる
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect_near(const point2d_t& pos, double radius, std::vector<int>& collected_tria_indexes) const;

    // 指定グリッド内に関連する三角形を全て取得し、indexの列としてcollected_tria_indexesに加える
    // posで指定した位置から半径radius内にある三角形(少しでもかすっていればOK)を全て取得する
    // posからradiusより遠い三角形も取ってくる可能性はあるが、radiusより近い三角形が除外されることはない
    // 入力のcollected_tria_indexesは昇順ソート済みである必要がある
    // その場合、結果のcollected_tria_indexesは昇順ソート済みのものとして得られる
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect_near(const std::vector<point2d_t>& pos, double radius, std::vector<int>& collected_tria_indexes) const;

    // 指定三角形に隣接する三角形を全て取得し、indexの列としてcollected_tria_indexesに加える
    // 入力のcollected_tria_indexesは昇順ソート済みである必要がある
    // その場合、結果のcollected_tria_indexesは昇順ソート済みのものとして得られる
    // 処理に成功すればtrueが返り、何かしらエラーが発生した場合falseが返る
    bool collect_connected(int tidx, std::vector<int>& collected_tria_indexes) const;
  public:
    ~grid_triangle_container();
    
    grid_triangle_container(const grid_triangle_container&);
    grid_triangle_container(grid_triangle_container&&);
    grid_triangle_container& operator = (const grid_triangle_container&);
    grid_triangle_container& operator = (grid_triangle_container&&);

  private:
    class impl;
    impl* pimpl;
  };
}
