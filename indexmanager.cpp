//128树，3层
//为何一定要B+树？好吧B+树查找效率的确高
//果然还是不想写B+树。。。写个二分查找算了
//文件中储存的仅仅是排好序的被index的键值
//只有在读出来之后才会被构建成树（无论何种）
//但构建的复杂度大概是O(n)
//除非命中缓存，否则和遍历没区别
//wwww