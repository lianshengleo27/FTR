<!-- title: 手順書 -->
## 1. バイナリ ファイルのBuild & 実行

### 1.1. シンボリックリンクの構築
---

> 「`diffelev_sample`」 フォルダの `diffelev` と 「`shared_sample`」 フォルダへのリンク

```bash
ln -s ../shared_object diffelev
```

### 1.2. ファイルのBuild
---
> In 「`diffelev_sample`」 フォルダ, type:
```
make
```
> バイナリの実行ファイル `sample_diffelev` を生成する

<!-- ![image](https://logowik.com/content/uploads/images/visual-studio-code7642.jpg) -->
<!-- <img src="https://logowik.com/content/uploads/images/visual-studio-code7642.jpg" alt="alt text" width="50" height="20"> -->


![image](images/1.png)


### 1.3. ファイルの実行
---

> In current directory where .exe file locates, type:

```
./sample_diffelev <--Option>
```

> 入力オプションの詳細を示す
```
Options:
  -p, --input-pcl       入力点群ファイルを指定します
  -g, --thin-grid       入力点群をグリッド毎に間引きます 計算方法を指定します
                        "maximum":グリッドのZ座標が最大値の点を残します
                        "minimum":グリッドのZ座標が最小値の点を残します
                        "median":グリッドのZ座標が中央値となる点を残します
                        "mode":グリッドのZ座標でヒストグラムを取った際の最瀕値となる点を残します
  -c, --cut-polygon     多角形を指定し、点群データからその内側にある点のみを抽出して残りを消去します
                        最初に多角形の点の数を指定し、続けて1点目のX座標、1点目のY座標、2点目以降...と指定します
  -n, --extract-near    設計データから近い点のみを抽出して残りを消去します
  -t, --thinned-pcl     間引かれたデータの出力先を指定します
  -o, --grid-origin     グリッドの基準位置を数値2つスペース区切りで設定します(初期値:0 0)
  -w, --grid-width      グリッドの感覚を数値2つスペース区切りで設定します(初期値:1 1)
  -s, --separator       入力ファイルの区切り文字を設定します(初期値:',')
  -i, --ignore-lines    入力ファイルの先頭何行を無視するかを指定します(初期値:0)
  -x, --input-landxml   設計データとなるLandXmlファイルを指定します
  -d, --diff-elevation  較差を計算し、指定したファイルに計算結果を出力します
                        各グリッドの較差算出結果のcsvファイル、全体の評価結果のjsonファイルの順に指定します
  -v, --standard-value  較差の計算で使用する規格値等のパラメータが書かれたjsonファイルを指定します
  -u, --out-used-value  較差の計算で使用した規格値等のパラメータが書かれたjsonファイルの出力先を指定します
  -a, --slope-angle     法角度を計算し、指定したファイルに計算結果を出力します
  -j, --judge-slope     評価する点群が天面か法面かを判定する計算方法を指定します
                        "force-top":必ず天面として計算します
                        "force-slope":必ず法面として計算します
                        "calculation":近接の設計データを計算で求め、その傾きで天面か法面かを決定します
  -h, --help            このヘルプを表示します
```


## 2. 出力結果

### 2.1. 事前設定
---
> グリッドの基準位置およびサイズの設定 (in m)

```py
# 点群密度を変更するため、まずグリッドを設置する

./sample_diffelev -o <defalut: 0 0> -w <default: 1 1>
```

> 入力点群ファイルの指定 `(-p)`
```py
#　指定されるパスから点群ファイルを読込む

./sample_diffelev -p <path/to/point_cloud_file>
```

> 設計データ(LandXML)ファイルの指定
```py
#　指定されるパスから設計データを読込む

./sample_diffelev -x <path/to/design_data_file>
```


### 2.2. 評価用点群の生成
---

> 範囲指定に応じた点群の切り出し
```py
# 範囲を指定する多角形の頂点および各頂点の座標を入力する

./sample_diffelev -c <多角形の頂点の数> <1点目のX座標> <1点目のY座標> <2点目以降...>
```  
<!----------------------------------------------------------------------------->

> 間引き方法の選択 & 出力先の指定
```py
# 各グリッドにおいて点群密度を減らし、代表点のみを抽出する

./sample_diffelev -g <"maximum" | "minimum" | "median" | "mode"> -t <path/to/output_file>
```
<!----------------------------------------------------------------------------->
> 設計データから±x m範囲の点群抽出 (in m)
```py
# 設計データから大いに離れる異常値を除外する

./sample_diffelev -n <default: 0.1>
```
<!----------------------------------------------------------------------------->

> 入力例
```py
# 点群ファイルdata_sample.txtと設計データdesign_data.xmlを読込み
# 各グリッドにおいて「最大値」、そして設計データから±0.1m範囲の点群のみを抽出する
# 出力先は、同じディレクトリの「output.csv」に保存する

./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -n 0.1 -t output.csv
```
`+resulted figure`


### 2.3. 標高較差 & ヒートマップ用データの算出
---
> 出力先およびファイルの形式を指定する
```py
# 「標高較差」は.csv、「ヒートマップ用データ」は.JSONファイルにそれぞれ保存する
./sample_diffelev -d diffelev.csv heatmap_data.json 
```

> 規格値の指定

* 事前に「**std_value.json**」というファイルを用意する

\>> `std_value.json`:
```json
{
    "ignore_distance": 0.05,
    "slope_average_threshold": 0.08,
    "slope_discard_rate": 0.003,
    "slope_maximum_threshold": 0.19,
    "slope_minimum_num_in_m3": 1,
    "slope_minimum_threshold": -0.19,
    "top_average_threshold": 0.05,
    "top_discard_rate": 0.003,
    "top_maximum_threshold": 0.15,
    "top_minimum_num_in_m3": 1,
    "top_minimum_threshold": -0.15,
    "top_slope_grad_threshold": 0.06
}
```


> 入力例
```py
# 規格値を指定する「std_value.json」ファイルをオプションとして入力

./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -d diffelev.csv heatmap_data.json -v std_value.json
```
> 結果

\>> `heatmap_data.json`:
```json
{
    "area": 305.0, //グリッド点数×グリッド面積
    "average": -4.245125128772781, //平均値：「評価用（間引き後）点群データ標高と設計値」の差の平均値（棄却点を除く）
    "discard": 301, //規格値を外れたデータ個数
    "maximum": 3.0552100422057435, //「評価用（間引き後）点群データ標高と設計値」の差の最大値（棄却点を除く）
    "minimum": -21.354345055558667, //「評価用（間引き後）点群データ標高と設計値」の差の最小値（棄却点を除く）
    "number": 305 //全評価用データ個数（棄却点を含む）
}
```

\>> `diffelev.csv`:

| 評価用データ座標Ｘ | 評価用データ座標Ｙ |  設計高  | 現況高  |  標高差  | 規格値比(%) |
| :----------------: | :----------------: | :------: | :-----: | :------: | :---------: |
|       -67082       |      -69394.6      | 234.827  | 236.322 | -0.00996 |  5.242138   |
|      -67082.6      |       -69395       | 235.7194 | 236.32  | -0.11943 |  62.85767   |
|      -67083.3      |      -69394.9      | 235.4363 | 236.301 | -0.00935 |  4.919924   |
|      -67082.9      |      -69395.4      | 234.5186 | 237.111 | 0.005401 |  2.842852   |
|      -67083.3      |      -69395.7      | 234.6022 | 237.27  | 0.003828 |  2.014805   |
|      -67085.5      |       -69394       | 236.4242 | 235.507 | -0.02623 |  13.80531   |
|      -67086.1      |       -69394       | 237.2012 | 235.42  | -0.03015 |  15.86919   |
|        ...         |        ...         |   ...    |   ...   |   ...    |     ...     |



### 2.4. 法角度の算出
---
> 天端か法面かを判定する計算方法を指定する
```sh
 ./sample_diffelev -j <"calculation" | "force-top" | "force-slope">

# "force-top":天端として計算します
# "force-slope":法面として計算します
# "calculation":設計データによる設計面の傾斜角度により、天端か法面かを判定する
```
> 出力先とファイルの形式を指定する
```
./sample_diffelev -a angle.json
```

> 入力例

```sh
./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -a angle.json -j "calculation"
```

> 結果（法角度を示す部分）


```json
//法面
"approx": {
    "angle": 37.76094288622349,   //法角度→近似法線とZ軸との角度(in deg)
    "center": [                   //法面の近似中心
        -67138.85045652174,
        -69419.96439130434,
        236.33465217391313
    ],
    "normal": [                   //法面の近似法線のベクトル
        0.03809997254399742,
        0.6111818920132767,
        0.7905726323161067
    ],
    "valid": "yes"
}
```








