type Matrix = [[Integer]]

-- parsem :: String -> Matrix

--parsem2 str = foldr op [] str
--          where op '\n' l = []:l
--                op x l = (x:(head l)):(tail l)

splitBy :: Char -> String -> [[Char]]
splitBy del = foldr op []
            where
                op :: Char -> [[Char]] -> [[Char]]
                op x [] = if x == del then [[]] else [[x]]
                op x (y:ys) = if x == del then []:(y:ys) else (x:y):ys


rd x = read x::Integer

-- takes a string and parses it to a Matrix
parsem = (map (map rd)).(map (splitBy ' ')).(splitBy '\n')

-- converts a matrix into a string
toString :: Matrix -> String
toString = (++ "\n").(g '\n').(map (g ' ')).f
    where
        f = map (map show)
        g del = foldr (\x acc -> if acc == [] then x else x++[del]++acc) []

displayMat = putStrLn . toString

-- the scalar product with an integer
vprod :: Integer -> Matrix -> Matrix
vprod x = map (map (* x))

-- adjoins two matrices by extending rows
hjoin = zipWith (++)

-- adjoins two matrices by adding new rows
vjoin m1 m2 = m1++m2

-- adds two matrices
msum = zipWith (zipWith (+))

-- transposition of a matrix
tr ([]:_) = []
tr m = (map head m) : (tr (map tail m))

-- the vectorial product of two matrices
mprod m1 m2 = map (prodline (tr m2)) m1
    where
        prodline m l = map (aij l) m
        aij li cj = foldr (+) 0 $ zipWith (*) li cj

{-
    1 2   1 2 3   09 12 15
    3 4 * 4 5 6 = 19 26 33
    5 6           29 40 51
-}

type Image = [[Char]]

l1="        ***** **            ***** **    "
l2="     ******  ****        ******  ****   "
l3="    **   *  *  ***      **   *  *  ***  "
l4="   *    *  *    ***    *    *  *    *** "
l5="       *  *      **        *  *      ** "
l6="      ** **      **       ** **      ** "
l7="      ** **      **       ** **      ** "
l8="    **** **      *      **** **      *  "
l9="   * *** **     *      * *** **     *   "
l10="      ** *******          ** *******    "
l11="      ** ******           ** ******     "
l12="      ** **               ** **         "
l13="      ** **               ** **         "
l14="      ** **               ** **         "
l15=" **   ** **          **   ** **         "
l16="***   *  *          ***   *  *          "
l17=" ***    *            ***    *           "
l18="  ******              ******            "
l19="    ***                 ***             "

logo = [l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,l14,l15,l16,l17,l18,l19]

-- image-displaying function
toStringImg :: Image -> String
toStringImg = (foldr (++) []).(map  (++ "\n"))

displaym = putStrLn . toStringImg

-- flips an image horizontally
flipH = map (reverse)

-- flips an image vertically
flipV = reverse

-- rotate 90grd clockwise
rotate90r = flipH . tr

-- rotate -90grd clockwise
rotate90l = tr . flipH

diamond n = (flipV (halfD n n)) ++ (halfD n (n-1))
    where
        -- bottom half:
        halfD n 0 = []
        halfD n stars = (lineD n stars):(halfD n (stars-1))
        -- one line:
        lineD n stars = (flipV (halflineD n stars)) ++ (tail (halflineD n stars))
        -- half a line:
        halflineD 0 _ = []
        halflineD n stars = if stars > 0 then '*':(halflineD (n-1) (stars-1))
                                   else ' ':(halflineD (n-1) 0)

overlay img mask = zipWith (zipWith imgAnd) img mask
    where
        imgAnd x y = if y == '*' then x else ' '

-- check : displaym $ overlay ["******","**  **","**  **","******"] [" **** ","******", " **** ", "  **  "]