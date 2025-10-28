/*******************************************************************************************
*
*   This program has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include "easings.h"

//----------------------------------------------------------------------------------
// Variables Definition
//----------------------------------------------------------------------------------
#define DEBUG 0
#define FIXED_COLUMNS_COUNT_MODE 0
#define RENDER_MODE 0 // 0 static + moving, 1 static only, 2 moving only

char* directoryPath = "D:/01 - SCINEMA/21 - Harry Potter/04 - Animation Assets/2 - Shots & Stills/HP3_small_shots";
//char* directoryPath = "D:/01 - SCINEMA/21 - Harry Potter/04 - Animation Assets/2 - Shots & Stills/HP2_small_shots";
//char* filePrefix = "HP3_AR3_ANIM/hp3_ar3_%s";

char* filePrefix = "HP3_AR1_ANIM/hp3_ar1_%s";
static int framerate = 60;
static float targetAspectRatio = 0.89f; // 1.12f or 0.89f
static int targetColumnsCount = 36;

// HP3
static int staticShots[] = { 9, 11, 13, 14, 15, 17, 18, 20, 21, 22, 23, 24, 28, 29, 30, 31, 32, 33, 34, 36, 37, 38, 40, 42, 43, 44, 45, 46, 47, 48, 50, 51, 52, 53, 54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70, 71, 74, 76, 80, 81, 83, 84, 86, 87, 91, 93, 95, 97, 100, 101, 102, 103, 115, 122, 130, 131, 132, 133, 135, 136, 138, 139, 141, 148, 150, 155, 166, 173, 176, 178, 179, 180, 181, 197, 198, 199, 202, 203, 204, 205, 206, 207, 208, 212, 213, 216, 218, 220, 221, 223, 226, 227, 228, 229, 230, 232, 233, 234, 235, 236, 237, 238, 239, 240, 243, 245, 246, 247, 249, 250, 251, 258, 260, 262, 265, 272, 275, 276, 278, 279, 280, 282, 283, 285, 289, 292, 293, 299, 300, 312, 313, 314, 315, 320, 322, 324, 326, 338, 341, 343, 371, 372, 373, 375, 376, 377, 392, 393, 398, 401, 402, 403, 405, 406, 407, 408, 409, 410, 422, 423, 424, 425, 428, 429, 431, 433, 434, 435, 436, 437, 439, 440, 441, 442, 444, 448, 449, 450, 453, 454, 457, 458, 463, 464, 468, 470, 476, 477, 478, 480, 483, 484, 485, 487, 493, 494, 513, 516, 517, 518, 522, 523, 524, 526, 527, 529, 530, 532, 533, 550, 551, 552, 553, 555, 557, 563, 564, 565, 568, 569, 571, 572, 573, 574, 579, 581, 582, 584, 595, 602, 605, 607, 611, 612, 613, 614, 628, 630, 631, 633, 634, 635, 640, 641, 654, 655, 656, 662, 665, 666, 667, 668, 669, 672, 677, 679, 681, 682, 686, 688, 691, 692, 713, 714, 737, 739, 742, 748, 754, 755, 756, 757, 758, 759, 761, 762, 764, 765, 766, 767, 768, 769, 773, 774, 779, 780, 782, 784, 785, 786, 792, 793, 794, 796, 799, 800, 802, 804, 810, 812, 814, 827, 839, 842, 843, 844, 847, 851, 852, 862, 880, 881, 899, 900, 901, 906, 907, 908, 909, 910, 928, 943, 946, 948, 950, 951, 952, 953, 954, 956, 957, 958, 960, 963, 969, 971, 972, 974, 975, 976, 986, 989, 996, 998, 1000, 1011, 1014, 1015, 1020, 1022, 1023, 1024, 1025, 1026, 1028, 1029, 1030, 1037, 1038, 1040, 1043, 1044, 1048, 1049, 1051, 1054, 1056, 1063, 1068, 1077, 1083, 1086, 1088, 1089, 1090, 1114, 1142, 1154, 1159, 1160, 1162, 1164, 1165, 1166, 1167, 1168, 1169, 1171, 1173, 1175, 1176, 1177, 1180, 1182, 1184, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1199, 1203, 1204, 1206, 1209, 1210, 1215, 1218, 1219, 1222, 1223, 1229, 1230, 1232, 1234, 1239, 1242, 1245, 1246, 1248, 1249, 1250, 1251, 1254, 1256, 1258, 1260, 1263, 1264, 1265, 1267, 1280, 1284, 1289, 1291, 1298, 1299, 1301, 1302, 1306, 1308, 1321, 1328, 1340, 1341, 1356, 1360, 1361, 1370, 1378, 1386, 1388, 1389, 1392, 1403, 1407, 1410, 1414, 1415, 1420, 1422, 1427, 1429, 1437, 1438, 1440, 1441, 1443, 1444, 1453, 1454, 1455, 1459, 1460, 1461, 1464, 1467, 1468, 1472, 1474, 1475, 1478, 1479, 1481, 1482, 1483, 1487, 1488, 1490, 1491, 1492, 1493, 1494, 1502, 1503, 1523, 1524, 1529, 1534, 1539, 1544, 1545, 1552, 1556, 1561, 1568, 1573, 1574, 1575, 1578, 1579, 1580, 1581, 1582, 1584, 1585, 1586, 1587, 1588, 1593, 1594, 1595, 1604, 1607 };

// HP2
//static int staticShots[] = { 2, 3, 4, 6, 8, 12, 13, 14, 16, 17, 18, 20, 22, 23, 24, 29, 30, 31, 32, 33, 34, 36, 37, 40, 41, 42, 44, 46, 47, 48, 49, 50, 53, 55, 57, 58, 59, 60, 61, 64, 67, 70, 71, 72, 73, 74, 75, 76, 78, 79, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 93, 94, 98, 100, 101, 102, 107, 108, 111, 112, 113, 116, 117, 118, 119, 120, 121, 123, 126, 128, 129, 130, 132, 133, 134, 135, 136, 137, 139, 140, 143, 146, 147, 149, 150, 151, 152, 153, 155, 156, 159, 160, 161, 165, 166, 169, 171, 173, 175, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 188, 189, 191, 194, 196, 197, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 210, 211, 212, 213, 215, 216, 217, 218, 219, 221, 222, 225, 226, 227, 228, 230, 231, 232, 233, 234, 235, 236, 237, 238, 240, 241, 242, 243, 245, 246, 247, 248, 250, 252, 253, 254, 255, 258, 259, 260, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 278, 279, 281, 282, 283, 284, 285, 286, 287, 288, 289, 291, 292, 293, 294, 296, 297, 298, 299, 300, 302, 304, 305, 306, 308, 309, 312, 313, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 343, 344, 345, 347, 348, 349, 351, 352, 353, 355, 360, 362, 363, 367, 368, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 387, 388, 390, 391, 393, 397, 398, 400, 401, 402, 403, 404, 405, 406, 408, 409, 410, 418, 420, 422, 423, 424, 427, 428, 429, 430, 433, 436, 437, 438, 439, 440, 450, 462, 471, 472, 473, 474, 476, 479, 480, 481, 482, 483, 486, 487, 488, 489, 490, 492, 493, 496, 497, 498, 500, 502, 503, 504, 505, 506, 507, 509, 510, 511, 513, 514, 515, 516, 517, 518, 519, 520, 524, 525, 526, 527, 528, 529, 532, 533, 534, 536, 538, 541, 542, 543, 544, 545, 546, 548, 550, 551, 555, 556, 557, 559, 561, 563, 564, 565, 566, 567, 568, 570, 571, 573, 574, 575, 576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 590, 591, 592, 593, 594, 595, 596, 598, 599, 600, 601, 603, 604, 605, 606, 607, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 627, 628, 629, 630, 631, 632, 633, 634, 635, 638, 639, 640, 641, 642, 644, 645, 646, 647, 648, 649, 651, 653, 654, 655, 656, 657, 659, 660, 662, 664, 665, 667, 669, 670, 671, 672, 673, 675, 677, 679, 680, 681, 683, 684, 685, 688, 695, 696, 697, 699, 700, 702, 703, 704, 705, 706, 707, 708, 711, 712, 713, 715, 716, 717, 718, 720, 721, 722, 724, 725, 726, 727, 728, 729, 731, 732, 733, 736, 737, 741, 742, 743, 744, 746, 747, 749, 750, 752, 753, 754, 755, 756, 757, 760, 761, 762, 764, 765, 766, 767, 769, 771, 772, 773, 774, 775, 776, 786, 787, 788, 789, 790, 791, 792, 793, 794, 795, 796, 797, 798, 799, 801, 802, 804, 805, 806, 807, 808, 809, 810, 812, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 839, 840, 841, 843, 844, 845, 846, 847, 849, 850, 852, 853, 854, 855, 858, 859, 860, 861, 863, 864, 866, 870, 871, 872, 873, 874, 875, 877, 878, 879, 880, 881, 883, 888, 890, 892, 893, 895, 897, 898, 899, 900, 902, 904, 906, 907, 919, 920, 936, 938, 939, 940, 944, 945, 949, 954, 959, 961, 962, 963, 964, 965, 976, 978, 979, 1022, 1032, 1033, 1036, 1040, 1041, 1045, 1049, 1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061, 1063, 1064, 1065, 1068, 1069, 1070, 1071, 1073, 1074, 1075, 1076, 1078, 1083, 1087, 1088, 1090, 1092, 1093, 1097, 1098, 1099, 1100, 1102, 1108, 1109, 1110, 1111, 1112, 1114, 1116, 1117, 1119, 1125, 1127, 1129, 1130, 1131, 1132, 1133, 1134, 1135, 1136, 1137, 1138, 1141, 1142, 1143, 1144, 1146, 1148, 1149, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1169, 1170, 1171, 1172, 1173, 1177, 1178, 1180, 1182, 1184, 1185, 1187, 1191, 1192, 1193, 1196, 1197, 1198, 1199, 1200, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1215, 1216, 1217, 1218, 1219, 1220, 1221, 1222, 1223, 1224, 1228, 1231, 1233, 1235, 1236, 1237, 1238, 1239, 1240, 1242, 1243, 1248, 1260, 1261, 1262, 1264, 1265, 1266, 1267, 1268, 1269, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1299, 1300, 1301, 1303, 1304, 1305, 1307, 1308, 1310, 1311, 1318, 1319, 1320, 1321, 1324, 1325, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1337, 1338, 1339, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1355, 1356, 1357, 1360, 1361, 1365, 1366, 1368, 1369, 1373, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383, 1386, 1390, 1392, 1396, 1402, 1403, 1404, 1409, 1412, 1414, 1415, 1417, 1419, 1420, 1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1444, 1445, 1446, 1447, 1449, 1450, 1453, 1455, 1460, 1461, 1462, 1463, 1464, 1465, 1466, 1468, 1473, 1474, 1477, 1479, 1480, 1482, 1483, 1485, 1486, 1488, 1489, 1490, 1492, 1494, 1495, 1496, 1497, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510, 1511, 1512, 1516, 1526, 1527, 1531, 1532, 1533, 1534, 1535, 1536, 1538, 1540, 1541, 1542, 1545, 1548, 1549, 1550, 1551, 1552, 1553, 1554, 1555, 1556, 1557, 1558, 1559, 1562, 1565, 1566, 1574, 1576, 1577, 1578, 1579, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1588, 1589, 1590, 1598, 1600, 1604, 1608, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1619, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1633, 1634, 1636, 1639, 1642, 1644, 1645, 1646, 1647, 1648, 1649, 1651, 1652, 1656, 1657, 1658, 1659, 1660, 1662, 1663, 1664, 1666, 1667, 1668, 1669, 1670, 1671, 1674, 1675, 1677, 1678, 1679, 1681, 1682, 1690, 1691, 1701, 1702, 1703, 1704, 1706, 1707, 1708, 1710, 1712, 1713, 1714, 1715, 1717, 1719, 1720, 1722, 1723, 1724, 1725, 1726, 1728, 1729, 1730, 1731, 1733, 1734, 1735, 1739, 1745, 1751, 1755, 1756, 1757, 1758, 1762, 1765, 1766, 1767, 1768, 1769, 1770, 1773, 1774, 1775, 1776, 1777, 1780, 1789, 1790, 1794, 1796, 1797, 1801, 1804, 1819, 1821, 1823, 1829, 1830, 1831, 1832, 1835, 1838, 1839, 1843, 1844, 1846, 1847, 1848, 1850, 1851, 1852, 1853, 1854, 1855, 1856, 1860, 1861, 1862, 1863, 1864, 1866, 1867, 1871, 1874, 1875, 1876, 1881, 1882, 1883, 1884, 1886, 1887, 1888, 1889, 1890, 1891, 1892, 1893, 1894, 1897, 1899, 1903, 1905, 1906, 1908, 1909, 1910, 1913, 1915, 1916, 1918, 1919, 1920, 1921, 1925, 1926, 1927, 1928, 1929, 1930, 1931, 1932, 1933, 1934, 1935, 1936, 1937, 1939, 1940, 1941, 1942, 1944, 1945, 1946, 1947, 1955, 1956, 1959, 1960, 1961, 1963, 1965, 1969, 1974, 1975, 1978, 1980, 1981, 1982, 1983, 1984, 1986, 1989, 1992, 1994, 1995, 1996, 1997, 1999, 2000, 2001, 2002, 2003, 2004, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2020, 2022, 2023, 2024, 2027, 2028, 2029, 2035, 2038, 2039, 2042, 2043, 2044, 2051, 2052, 2053, 2054, 2055, 2056, 2059, 2060, 2061, 2070, 2071, 2074, 2075, 2079, 2081, 2084, 2088, 2089, 2090, 2091, 2092, 2094, 2100, 2102, 2103, 2105, 2107, 2119, 2121, 2122, 2123, 2124, 2129, 2130, 2132, 2133, 2134, 2138, 2151, 2152, 2154, 2165, 2167, 2180, 2181, 2185, 2190, 2192, 2194, 2199, 2204, 2205, 2207, 2208, 2209, 2211, 2215, 2224, 2226, 2228, 2229, 2231, 2233, 2235, 2236, 2237, 2238, 2245, 2246, 2248, 2250, 2251, 2254, 2255, 2256, 2258, 2260, 2261, 2262, 2263, 2264, 2265, 2266, 2267, 2268, 2270, 2271, 2275, 2276, 2277, 2278, 2279, 2280, 2281, 2282, 2283, 2284, 2285, 2286, 2289, 2290, 2292, 2293, 2295, 2296, 2297, 2299, 2300, 2302, 2303, 2306, 2307, 2310, 2311, 2312, 2317, 2318, 2319, 2320, 2321, 2322, 2324, 2325, 2326, 2327, 2328, 2329, 2330, 2331, 2332, 2333, 2334, 2335, 2339, 2340, 2341, 2344, 2347, 2348, 2349, 2352, 2355, 2356, 2357, 2358, 2359, 2360, 2364, 2366, 2367, 2369, 2372, 2375, 2381, 2382, 2383, 2384, 2385, 2386, 2387, 2391, 2392, 2393, 2394, 2395, 2396, 2398, 2401, 2402, 2405, 2408, 2409, 2410, 2411, 2412, 2414, 2415, 2416, 2417, 2421, 2422, 2423, 2424, 2425, 2426, 2427, 2428, 2430, 2431, 2432, 2433, 2434, 2436, 2439, 2440, 2441, 2442, 2446, 2447, 2448, 2451, 2452, 2453, 2456, 2457, 2458, 2459, 2460, 2461, 2467, 2468, 2473, 2476, 2477, 2478, 2481, 2483, 2486 };



//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
#define maxTextureCount 5000
static int shotsCount = 0;

static int textureWidth = 0;
static int textureHeight = 0;
static float textureScale = 0;

static float stepWidth = 0;
static float stepHeight = 0;
static float objectSpacing = 8.f;

static int columnsCount = 0;
static int rowsCount = 0;

static Texture2D shotsTextures[maxTextureCount];
static RenderTexture2D bufferCanvas;

static const int screenWidth = 1280;
static const int screenHeight = 720;

static int renderWidth;
static int renderHeight;

static int render_pass = 0;

//----------------------------------------------------------------------------------

static int animPlayerLength = 168;

static int yAnimOffset = 250;
static int yAnimLength = 48;
static int yAnimEaseType = 1;

static int fadeAnimLength = 56;
static int fadeEaseType = 1;

static float perObjectShift = 1000.f; // 500.f per default
static float perObjectShiftPower = 2.f;

static float objectStrokeWidth = 0.f;
static Color objectStrokeColor = {
    253, 249, 0, 255
};

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ClearRenderTexture(RenderTexture2D* target);
static void WriteImageFromTexture(Texture2D* texture, const char* filename);
static void InitRenderParameters(char* filepathImg);
static void ComputeLayout();
static int ComputeColumnCount(int renderWidth, int renderHeight, int textureWidth, int textureHeight);
static void DrawAnimTexture(int frameIndex);
static void ButtonExport(int animLength);

static bool IsStaticShot(int shotIndex) {
    for (size_t i = 0; i < sizeof(staticShots) / sizeof(staticShots[0]); i++) {
        if (staticShots[i] == shotIndex+1)
            return true;
    }
    return false;
}

static bool IsInArrayDebug(int val, int *arr) {
    size_t arrLen = sizeof(*arr) / sizeof(arr[0]);
    printf(TextFormat("%d\n", arrLen));

    for (size_t i = 0; i < arrLen; i++) {
        printf(TextFormat("checking against %d\n", i));

        if (arr[i] == val)
        {
            printf(TextFormat("%d is in array\n", val));
            return true;
        }
    }
    printf(TextFormat("%d is not in array\n", val));
    return false;
}

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TRANSPARENT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Movie Shots Maker");

    FilePathList shotsFilePaths = LoadDirectoryFiles(directoryPath);
    shotsCount = shotsFilePaths.count;

    char* filepathFirstImage = shotsFilePaths.paths[0];
    InitRenderParameters(filepathFirstImage);

    bufferCanvas = LoadRenderTexture(renderWidth, renderHeight + yAnimOffset);

#if (DEBUG == 0)
    for (unsigned int i = 0; i < shotsCount; i++)
    {
        char* filepath = shotsFilePaths.paths[i];
        shotsTextures[i] = LoadTexture(filepath);
        //Image img = LoadImage(filepath);
        //ImageResize(&img, (int)(textureWidth * 0.2f), (int)(textureHeight * 0.2f));
        //UnloadImage(img);
    }
#endif // DEBUG == 0
    

    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    bool windowBoxMainActive = true;
    bool animPlayerLengthValueBoxEditMode = false;
    bool yAnimOffsetValueBoxEditMode = false;
    bool yAnimLengthValueBoxEditMode = false;
    bool yAnimEaseDropdownBoxEditMode = false;
    bool fadeAnimLengthValueBoxEditMode = false;
    bool fadeEaseDropdownBoxEditMode = false;

    char* easingTypes = "OUT_SIN;OUT_CUBIC";

    bool animPlayerIsPlaying = true;
    float animPlayerFrame = 0.f;
    //----------------------------------------------------------------------------------

    SetTargetFPS(framerate);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector2 anchorWindow = { GetScreenWidth() - 304, 0 };
        Vector2 anchorLayoutBox = { anchorWindow.x + 16, anchorWindow.y + 48 }; // 592, 96
        Vector2 anchorAnimBox = { anchorWindow.x + 16, anchorWindow.y + 256 };
        Vector2 anchorSequenceBox = { anchorWindow.x + 16, anchorWindow.y + 496 + 48 };

        if (animPlayerIsPlaying)
        {
            if (animPlayerFrame < animPlayerLength) {
                animPlayerFrame += 1.f;
            }
            else
            {
                animPlayerFrame = 0.f;
            }
        }


        //----------------------------------------------------------------------------------
        // Draw in render buffer
        //----------------------------------------------------------------------------------
        ComputeLayout();
        DrawAnimTexture(animPlayerFrame);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            ClearBackground(BLACK);

            // Draw render texture
            float aspectRatio = (float)renderHeight / (float)renderWidth;
            int previewWidth = min(GetScreenWidth(), (int)((float)GetScreenHeight() / aspectRatio));
            DrawTexturePro(bufferCanvas.texture, (Rectangle) { 0, 0, (float)bufferCanvas.texture.width, (float)-bufferCanvas.texture.height }, (Rectangle) { 0, 0, previewWidth, previewWidth * aspectRatio }, (Vector2) { 0, 0 }, 0.f, WHITE);
            
            {
                // raygui: controls drawing
                //----------------------------------------------------------------------------------
                if (windowBoxMainActive)
                {
                    int baseAnimPlayerSize = 24;

                    windowBoxMainActive = !GuiWindowBox((Rectangle) { anchorWindow.x + 0, anchorWindow.y + 0, 304, GetScreenHeight() - baseAnimPlayerSize }, "OPTIONS");
                    
                    if (yAnimEaseDropdownBoxEditMode || fadeEaseDropdownBoxEditMode) GuiLock();

                    // Layout settings
                    GuiGroupBox((Rectangle) { anchorLayoutBox.x + 0, anchorLayoutBox.y + 0, 272, 176 }, "Layout settings");
                    GuiSliderBar((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 16, 120, 16 }, TextFormat("Spacing (%.1f)", objectSpacing), NULL, & objectSpacing, 0.f, 50.f);
                    GuiSliderBar((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 40, 120, 16 }, "Stroke Width (0-10)", NULL, & objectStrokeWidth, 0, 10.f);
                    GuiLabel((Rectangle) { anchorLayoutBox.x + 32, anchorLayoutBox.y + 104, 88, 24 }, "Stroke Color");
                    GuiColorPicker((Rectangle) { anchorLayoutBox.x + 136, anchorLayoutBox.y + 72, 88, 88 }, "Stroke Color", & objectStrokeColor);

                    // Anim settings
                    GuiGroupBox((Rectangle) { anchorAnimBox.x + 0, anchorAnimBox.y + 0, 272, 208 + 48 }, "Animation settings");
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 16, 120, 24 }, "Y Animation Length ", & yAnimLength, 0, animPlayerLength, yAnimLengthValueBoxEditMode)) yAnimLengthValueBoxEditMode = !yAnimLengthValueBoxEditMode;
                    GuiLabel((Rectangle) { anchorAnimBox.x + 32, anchorAnimBox.y + 48, 120, 24 }, "Y Animation Ease");
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 80, 120, 24 }, "Y Animation Offset ", & yAnimOffset, 0, 2000, yAnimOffsetValueBoxEditMode)) yAnimOffsetValueBoxEditMode = !yAnimOffsetValueBoxEditMode;
                    
                    GuiLine((Rectangle) { anchorAnimBox.x + 16, anchorAnimBox.y + 112, 240, 16 }, NULL);
                
                    if (GuiSpinner((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 136, 120, 24 }, "Fade Length ", & fadeAnimLength, 0, animPlayerLength, fadeAnimLengthValueBoxEditMode)) fadeAnimLengthValueBoxEditMode = !fadeAnimLengthValueBoxEditMode;
                    GuiLabel((Rectangle) { anchorAnimBox.x + 64, anchorAnimBox.y + 168, 80, 24 }, "Fade Ease");

                    GuiLine((Rectangle) { anchorAnimBox.x + 16, anchorAnimBox.y + 200, 240, 16 }, NULL);

                    GuiSliderBar((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 224, 120, 16 }, TextFormat("Per object shift (%d)", (int)perObjectShift), NULL, & perObjectShift, 0.f, 2000.f);
                    
                    // Sequence settings
                    GuiGroupBox((Rectangle) { anchorSequenceBox.x + 0, anchorSequenceBox.y + 0, 272, 56 }, "Sequence settings");
                    if (GuiSpinner((Rectangle) { anchorSequenceBox.x + 136, anchorSequenceBox.y + 16, 120, 24 }, "Length (frames) ", & animPlayerLength, 0, 100000, animPlayerLengthValueBoxEditMode)) animPlayerLengthValueBoxEditMode = !animPlayerLengthValueBoxEditMode;
                    
                    if (GuiButton((Rectangle) { anchorWindow.x + 16, anchorWindow.y + 578 + 48, 272, 32 }, "Export animation")) ButtonExport(animPlayerLength);
                    
                    if (GuiDropdownBox((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 48, 120, 24 }, easingTypes, & yAnimEaseType, yAnimEaseDropdownBoxEditMode)) yAnimEaseDropdownBoxEditMode = !yAnimEaseDropdownBoxEditMode;
                    if (GuiDropdownBox((Rectangle) { anchorAnimBox.x + 136, anchorAnimBox.y + 168, 120, 24 }, easingTypes, & fadeEaseType, fadeEaseDropdownBoxEditMode)) fadeEaseDropdownBoxEditMode = !fadeEaseDropdownBoxEditMode;

                    // Animation player
                    GuiStatusBar((Rectangle) { 0, GetScreenHeight() - baseAnimPlayerSize, 48, baseAnimPlayerSize }, TextFormat("%d", (int)animPlayerFrame));
                    if (GuiButton((Rectangle) { 48, GetScreenHeight() - baseAnimPlayerSize, baseAnimPlayerSize, baseAnimPlayerSize }, TextFormat("#%i#", animPlayerIsPlaying ? ICON_PLAYER_PAUSE : ICON_PLAYER_PLAY))) animPlayerIsPlaying = !animPlayerIsPlaying;
                    if (GuiButton((Rectangle) { 48 + baseAnimPlayerSize, GetScreenHeight() - baseAnimPlayerSize, baseAnimPlayerSize, baseAnimPlayerSize }, TextFormat("#%i#", ICON_RESTART))) animPlayerFrame = 0;
                    GuiSlider((Rectangle) { 48 + baseAnimPlayerSize * 2, GetScreenHeight() - baseAnimPlayerSize, GetScreenWidth() - baseAnimPlayerSize * 2, baseAnimPlayerSize }, NULL, NULL, &animPlayerFrame, 0.f, (float)animPlayerLength);
                }

                GuiUnlock();
                //----------------------------------------------------------------------------------
            }

            DrawFPS(10, 10);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload current screen data before closing
    for (int i = 0; i < shotsCount; i++)
    {
        UnloadTexture(shotsTextures[i]);
    }
    UnloadRenderTexture(bufferCanvas);

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
static void ClearRenderTexture(RenderTexture2D* target)
{
    BeginTextureMode(*target);
    ClearBackground(BLANK);
    EndTextureMode();
}

static void WriteImageFromTexture(Texture2D* texture, const char* filename)
{
    Image imgToExport = LoadImageFromTexture(*texture);
    ExportImage(imgToExport, TextFormat("./export/%s", filename));
    UnloadImage(imgToExport);
}

static void InitRenderParameters(char* filepathImg)
{   
    renderHeight = 1080 * 2;
    renderWidth = (int)floorf((float)renderHeight * targetAspectRatio);

    Image img = LoadImage(filepathImg);
    textureWidth = img.width;
    textureHeight = img.height;
    UnloadImage(img);

    ComputeLayout();
}

static void ComputeLayout()
{
#if (FIXED_COLUMNS_COUNT_MODE == 0)
    int computedColumnCount = ComputeColumnCount(renderWidth, renderHeight, textureWidth, textureHeight);

    if (computedColumnCount != columnsCount)
    {
        columnsCount = ComputeColumnCount(renderWidth, renderHeight, textureWidth, textureHeight);
        rowsCount = (int)ceilf((float)shotsCount / (float)columnsCount);

        stepWidth = ((float)renderWidth - (float)(columnsCount - 1) * objectSpacing) / (float)columnsCount;
        textureScale = stepWidth / (float)textureWidth;
        stepHeight = textureScale * (float)textureHeight;

        printf(TextFormat("---\nColumns: %d\nRows: %d\nShots: %d / %d\n---\n", columnsCount, rowsCount, columnsCount * rowsCount, shotsCount));
    }
#else
    columnsCount = targetColumnsCount;
    rowsCount = (int)ceilf((float)shotsCount / (float)columnsCount);
    stepWidth = (int)floorf(((float)renderWidth - (float)(columnsCount - 1) * objectSpacing) / (float)columnsCount);
    stepHeight = (int)ceilf(((float)stepWidth / (float)textureWidth) * (float)textureHeight);
    textureScale = ((float)stepWidth / (float)textureWidth);

    int newRenderHeight = (int)((stepHeight + objectSpacing) * rowsCount - objectSpacing) + 200;

    if (newRenderHeight != renderHeight)
    {
        renderHeight = newRenderHeight;
        UnloadRenderTexture(bufferCanvas);
        bufferCanvas = LoadRenderTexture(renderWidth, renderHeight);

        printf(TextFormat("---\nColumns: %i\nRows: %i\nShots: %i / %i\n---\n", columnsCount, rowsCount, columnsCount * rowsCount, shotsCount));
        printf(TextFormat("---\nWidth: %i, Height: %i\nAR: %f\n---\n", renderWidth, renderHeight - 200, (float)renderWidth / (float)(renderHeight - 200)));
    }
#endif // (FIXED_COLUMNS_COUNT_MODE == 0)
}

static int ComputeColumnCount(int _renderWidth, int _renderHeight, int _textureWidth, int _textureHeight)
{
    
    for (unsigned int i = 1; i < 200; i++)
    {
        float width = ((float)_renderWidth - (float)(i - 1) * objectSpacing) / (float)i;
        float texScale = (float)width / (float)_textureWidth;
        float height = texScale * (float)_textureHeight;
        
        int _rowsCount = (int)ceilf( (float)shotsCount / (float)i );
        float y = (float)(_rowsCount + 1) * (height + objectSpacing) + objectSpacing + 0.f*(float)yAnimOffset/2.f;

        if (y < (float)_renderHeight)
        {
            return i;
        }
        else
        {
            continue;
        }
    }
}

static void DrawAnimTexture(int frameIndex)
{
    ClearRenderTexture(&bufferCanvas);
    BeginTextureMode(bufferCanvas);
    {
#if (DEBUG == 1)
        ClearBackground(GRAY);
#endif // DEBUG        
        for (unsigned int i = 0; i < shotsCount; i++)
        {
#if (RENDER_MODE == 0)
            bool isStatic = IsStaticShot(i);
            if ((render_pass == 1 && !isStatic) || (render_pass == 2 && isStatic))
            {
                continue;
            }
#elif (RENDER_MODE == 1)
            if (!IsStaticShot(i))
            {
                continue;
            }
#elif (RENDER_MODE == 2)
            if (IsStaticShot(i))
            {
                continue;
            }

#endif // (RENDER_MODE == 0)            

            float elementAnimKeyframe = frameIndex - i * pow(perObjectShift / 5000.f, 2.f);
            float offset = objectStrokeWidth * 0.5f;

            float fadeAnimInterpolator = Clamp(elementAnimKeyframe / (float)fadeAnimLength, 0.f, 1.f);
            float fadeAnimProgress = fadeEaseType == 0 ? EaseOutSin(fadeAnimInterpolator) : EaseOutCubic(fadeAnimInterpolator);

            float opacity = Lerp(0, 255, fadeAnimProgress);

            float yAnimInterpolator = Clamp(elementAnimKeyframe / (float)yAnimLength, 0.f, 1.f);
            float yAnimProgress = yAnimEaseType == 0 ? EaseOutSin(yAnimInterpolator) : EaseOutCubic(yAnimInterpolator);

            int columnIndex = i % columnsCount;
            int rowIndex = (int)floorf((float)i / (float)columnsCount);

            float x = (float)columnIndex * (stepWidth + objectSpacing);
            //int y = (rowIndex) * (stepHeight + objectSpacing) + Lerp((float)yAnimOffset, 0.f, yAnimProgress) + offset * 2.f;
            float y = objectSpacing + (float)rowIndex * (stepHeight + objectSpacing) + Lerp((float)yAnimOffset, 0.f, yAnimProgress) + offset * 2.f;

            int width = textureWidth * textureScale;
            int height = textureHeight * textureScale;

            Rectangle rectObject = { x, y, width, height };
#if (DEBUG == 0)
            Color tintColor = { WHITE.r, WHITE.g, WHITE.b, opacity };
            DrawTexturePro(shotsTextures[i], (Rectangle) { 0, 0, textureWidth, textureHeight }, rectObject, (Vector2) { 0, 0 }, 0.f, tintColor);
            //DrawTexture(shotsTextures[i], x, y, WHITE);
#else
            DrawRectangleRec(rectObject, (Color) { RED.r, RED.g, RED.b, opacity });
#endif // DEBUG
            
            if (objectStrokeWidth > 0.f)
            {
                Color strokeColor = { objectStrokeColor.r, objectStrokeColor.g, objectStrokeColor.b, opacity };
                DrawRectangleLinesEx((Rectangle) { rectObject.x - offset, rectObject.y - offset, rectObject.width + offset * 2.f, rectObject.height + offset * 2.f }, objectStrokeWidth, strokeColor);
            }
        }
    }
    EndTextureMode();
}

static void ButtonExport(int animLength)
{
    RenderTexture2D bufferRender = LoadRenderTexture(renderWidth, renderHeight + yAnimOffset);
#if (RENDER_MODE == 0)
        for (unsigned int i = 0; i < 2; i++)
        {
            render_pass = i+1;
#endif // (RENDER_MODE == 0)
            for (unsigned int j = 0; j < animLength; j++)
            {
                DrawAnimTexture(j);
                ClearRenderTexture(&bufferRender);
                BeginTextureMode(bufferRender);
                {
                    DrawTexture(bufferCanvas.texture, 0, 0, WHITE);
                }
                EndTextureMode();

                if (render_pass == 0)
                {
                    WriteImageFromTexture(&bufferRender.texture, TextFormat(filePrefix, TextFormat("anim_%05d.png", j)));
                }
                else if (render_pass == 1)
                {
                    WriteImageFromTexture(&bufferRender.texture, TextFormat(filePrefix, TextFormat("anim_staticshots_%05d.png", j)));
                }
                else if (render_pass == 2)
                {
                    WriteImageFromTexture(&bufferRender.texture, TextFormat(filePrefix, TextFormat("anim_movingshots_%05d.png", j)));
                }
            }
#if (RENDER_MODE == 0)
        }
#endif // (RENDER_MODE == 0)

    render_pass = 0;
}
