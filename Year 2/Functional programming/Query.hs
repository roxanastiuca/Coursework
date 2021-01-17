module Query where

import UserInfo
import Rating
import Movie

import Data.List
import Numeric

type Column = String
type TableSchema = [Column]
type Field = String
type Entry = [Field]

data Table = Table TableSchema [Entry]

type ColSeparator = Char
type LnSeparator = Char

-- TODO 1

{--
    Creates a table from a string.
--}
read_table :: ColSeparator -> LnSeparator -> String -> Table
read_table colDel lnDel str = Table (read_tableschema colDel lnDel str) 
                            (tail $ init (read_entries colDel lnDel str))

{--
    Creates a table's header by separating the first line by columns.
--}
read_tableschema :: ColSeparator -> LnSeparator -> String -> TableSchema
read_tableschema colDel lnDel str = splitBy colDel (oneSplitBy lnDel str)

{--
    Creates a table's entries' list by separating lines and columns;
    First, it separates each line into a different String.
    Then it maps the list of String into a list of list of Strings,
    by separating each line's columns.
--}
read_entries :: ColSeparator -> LnSeparator -> String -> [Entry]
read_entries colDel lnDel str = map (splitBy colDel) (splitBy lnDel str)

{--
    Splitting function extracted from pdmatei github.
    Takes a delimitator character and a string and returns a list of strings.
--}
splitBy :: Char -> String -> [String]
splitBy c = foldr op [[]]
    where op x (y:ys)
            | x /= c = (x:y):ys
            | otherwise = []:(y:ys)

{--
    Similar to the splitting function above, but stops at the delimitator
    and returns the string until that.
    It is used for reading the header, so that it doesn't parse the whole
    text when only the first part is needed.
--}
oneSplitBy :: Char -> String -> String
oneSplitBy c [] = []
oneSplitBy c (x:xs) = if x == c then [] else x:(oneSplitBy c xs)

user_info = read_table '|' '\n' user_info_str
rating = read_table ' ' '\n' rating_str
movie = read_table '|' '\n' movie_str

-- TODO 2

{--
    Enrolls Table in the Show class.
    Calls auxiliar function which also has as parameter the size of each
    column.
--}
instance Show Table where
    show table = show_table (get_col_size table) table

{--
    Creates list of integer where at index i we have the maximum size of
    an element in the i-th column.
    That can be achieved with a fold through each entry where
    - the accumulator is the list of the sizes of the header's elements.
    - the function applied on acc and entry is a zipWith (to parse every
    column) that computes the maximum of each column.
--}
get_col_size :: Table -> [Int]
get_col_size (Table header entries) = foldr compute_max (headers_size header) entries
    where
        compute_max = zipWith (\x y -> max (length x) y)
        headers_size header = foldr (\x acc -> (length x):acc) [] header

{--
    Converts a table into a String, by adding padding to columns.
    For converting the entries, it uses a fold function that adds each
    column + padding to a string.
--}
show_table :: [Int] -> Table -> String
show_table col_sizes (Table header entries) =
                        (show_border col_sizes) ++
                        "|" ++ (show_line col_sizes header) ++
                        (show_border col_sizes) ++
                        (foldr (\x acc -> "|" ++ (show_line col_sizes x) ++ acc) [] entries) ++
                        (show_border col_sizes)

{--
    Function which creates a string only with '-' for the border of the table.
--}
show_border :: [Int] -> String
show_border sizes = (take (1 + (length sizes) + (foldr (+) 0 sizes)) (repeat '-'))++"\n"

{--
    Function which convert an entry into a line by adding padding to each column.
--}
show_line :: [Int] -> [String] -> String
show_line [] [] = "\n"
show_line (len:xs) (str:ys) =
            str ++ (take (len - (length str)) (repeat ' ')) ++ 
            "|" ++ (show_line xs ys)
show_line _ _ = []

data FilterCondition = Lt Field Integer | Eq Field String | In Field [String] | Not FilterCondition

-- TODO 3

{--
    Function which converts a filter condition applied on table with a certain header to
    a function which receives a table entry and returns whether that entry fits the condition
    or not.
--}
getFilter :: FilterCondition -> TableSchema -> (Entry -> Bool)
getFilter (Lt field nr) header = (\entry -> stringToInteger (entry !! (getIndex field header)) < nr)
getFilter (Eq field str) header = (\entry -> (entry !! (getIndex field header)) == str)
getFilter (In field list) header = (\entry -> elem (entry !! (getIndex field header)) list)
getFilter (Not cond) header = (\entry -> not (getFilter cond header entry))

{--
    Returns an element's index into a list.
--}
getIndex :: Column -> [Column] -> Int
getIndex elem list = maybeToInt (elemIndex elem list) 

{--
    Converts a Maybe Int to an Int (for Nothing, it returns -1).
--}
maybeToInt :: (Maybe Int) -> Int
maybeToInt (Just x) = x
maybeToInt Nothing = -1

{--
    Converts a String to an Integer
--}
stringToInteger :: String -> Integer
stringToInteger str = read str :: Integer

-- TODO 4

data Query = Filter FilterCondition Query |  
             Select [String] Query |
             SelectLimit [String] Integer Query |
             Cosine Query |
             Query :|| Query |
             Atom Table

{--
    Function which evaluates a query to a table.
--}
eval :: Query -> Table
eval (Atom table) = table
eval (Select fields query) = select_fields fields (eval query)
eval (SelectLimit fields limit query) = select_fields fields $
                                limit_entries (fromIntegral limit) (eval query)
eval (Filter condition query) = filter_entries condition (eval query)
eval (query1 :|| query2) = table_reunion (eval query1) (eval query2)
eval (Cosine query) = find_cosine (eval query)

{--
    Function which selectes certain columns from a table.
--}
select_fields :: [String] -> Table -> Table
select_fields fields (Table header entries) = Table fields
            (select_entries_fields (getIndexes fields header) entries)

{--
    Creates list of integer where the i-th element is the index in
    header where the i-th string was found.
--}
getIndexes :: [String] -> [String] -> [Int]
getIndexes [] _ = []
getIndexes (x:xs) header = (getIndex x header):(getIndexes xs header)

{--
    Converts a list of entries into another with only certain columns (given
    by their index).
--}
select_entries_fields :: [Int] -> [Entry] -> [Entry]
select_entries_fields indexes entries = map (select_entry_fields indexes) entries

{--
    Selects from an entry only the columns with an index found in a list.
--}
select_entry_fields :: [Int] -> Entry -> Entry
select_entry_fields [] _ = []
select_entry_fields (index:xs) entry = (entry !! index):(select_entry_fields xs entry)

{--
    Creates new table with only the first 'limit' entries.
--}
limit_entries :: Int -> Table -> Table
limit_entries limit (Table header entries) = Table header (take limit entries)

{--
    Filters the list of entries based on the condition.
--}
filter_entries :: FilterCondition -> Table -> Table
filter_entries cond (Table header entries) = Table header (filter (getFilter cond header) entries)

{--
    Creates the union between two tables with the same header.
--}
table_reunion :: Table -> Table -> Table
table_reunion (Table header1 entries1) (Table header2 entries2) = Table header1 (entries1 ++ entries2)

-- TODO 5

{--
    Illustrates the eval function. Selects entries from a table with the same
    zone as a given user.
--}
same_zone :: String -> Query
same_zone user_id =
            Filter (Not (Eq "user_id" user_id)) $
            Select ["user_id", "occupation"] $
            Filter (Eq "zone" (get_user_zone user_id)) $
            Atom user_info

{--
    Gets a user's "zone" field value.
--}
get_user_zone :: String -> String
get_user_zone user_id =
            (extract_fst_entry $ eval $
            Select ["zone"] $
            Filter (Eq "user_id" user_id) $
            Atom user_info) !! 0

{--
    Extracts the first entry from a table.
--}
extract_fst_entry :: Table -> Entry
extract_fst_entry (Table header entries) = (entries !! 0)

{--
    Illustrates the eval function. Selects entries of only male within a
    certain age.
--}
male_within_age :: Integer -> Integer -> Query
male_within_age age_min age_max =
            Select ["occupation", "zone"] $
            Filter (Lt "age" age_max) $
            Filter (Not (Lt "age" age_min)) $
            Filter (Not (Eq "age" (show age_min))) $
            Filter (Eq "sex" "M") $
            Atom user_info

{--
    Illustrates the eval function. Selects entries within an age, occupation
    and zone.
--}
mixed :: [String] -> [String] -> Integer -> Query
mixed zones occupations age_max =
            Select ["user_id"] $
            Filter (In "zone" zones) $
            Filter (In "occupation" occupations) $
            Filter (Lt "age" age_max) $
            Atom user_info

-- BONUS

{--
    Returns the table with the similarities between each 2 users.
--}
find_cosine :: Table -> Table
find_cosine (Table header entries) = Table new_header (find_sim (sort entries))

{--
    The header of the similarities table.
--}
new_header :: TableSchema
new_header = ["user_id1", "user_id2", "sim"]

{--
    Returns the list of entries with the similarities between each 2 users.
--}
find_sim :: [Entry] -> [Entry]
find_sim entries = compute_cosine 
    (init_sim_entries (remove_duplicates (map (!! 0) entries)))
    (find_ratings (remove_duplicates (map (!! 0) entries)) (remove_duplicates (map (!! 1) entries)) entries)

{--
    Removes duplicates from a list and returns it sorted.
--}
remove_duplicates :: Ord a => [a] -> [a]
remove_duplicates = map head . group . sort

{--
    Receives a list of all users and returns a list of entries where the first 2 fields are
    each 2 users from the list.
--}
init_sim_entries :: [String] -> [Entry]
init_sim_entries [] = []
init_sim_entries (x:xs) = (init_id_entries x xs)++(init_sim_entries xs)

{--
    For an user_id, it creates the list of entries where the first field, "user_id1" is him,
    the second is each of the user-ids bigger then him and the third is an empty string.
--}
init_id_entries :: String -> [String] -> [Entry]
init_id_entries id [] = []
init_id_entries id (x:xs) = ([id, x, []]):(init_id_entries id xs)

{--
    For every user, it creates a list of his ratings on all movies.
    Input: list of user_ids, list of movie_ids, all entries in the initial table
    Output: A list of pairs: (user_id, list of ratings for all movies)
--}
find_ratings :: [String] -> [String] -> [Entry] -> [(String, [Float])]
find_ratings [] movie_list entries = []
find_ratings (id:ids) movie_list entries = 
    (id, (find_id_ratings movie_list (filter (\entry -> (entry !! 0) == id) entries))):
    (find_ratings ids movie_list entries)

{--
    For a user, it creates a list of his ratings on all movies, by parsing through the
    movies' list.
--}
find_id_ratings :: [String] -> [Entry] -> [Float]
find_id_ratings [] entries = []
find_id_ratings (movie:movies) entries =
    (find_movie_rating movie entries):
    (find_id_ratings movies entries)

{--
    Find in a list of entries, the one for a given movie and returns 0 if not found or
    the float value of the "rating" field.
--}
find_movie_rating :: String -> [Entry] -> Float
find_movie_rating movie entries = 
    if ((filter (\entry -> (entry !! 1) == movie) entries) == [])
        then 0
        else (read ((head (filter (\entry -> (entry !! 1) == movie) entries)) !! 2) :: Float)

{--
    Computes the cosine for the ratings of each 2 users.
    Input: the list of entries such as [user_id1, user_id2, ""] where the third element is yet to be
        added, the list of pairs (user_id, list of ratings).
    Output: the list of entries in the similarities table. 
--}
compute_cosine :: [Entry] -> [(String, [Float])] -> [Entry]
compute_cosine [] ratings = []
compute_cosine ([user1, user2, rating]:xs) ratings =
    [user1, user2, (floatToString (cosine (get_ratings user1 ratings) (get_ratings user2 ratings)))]:
    (compute_cosine xs ratings)

{--
    Gets list of ratings of a certain user.
--}
get_ratings :: String -> [(String, [Float])] -> [Float]
get_ratings user ratings = snd (head (filter (\x -> (fst x) == user) ratings))

{--
    Converts float to string, showing exactly 4 decimals.
--}
floatToString :: Float -> String
floatToString nr = showFFloat (Just 4) nr ""

{--
    Computes the cosine for 2 arrays.
--}
cosine :: [Float] -> [Float] -> Float
cosine arr1 arr2 = (scalar_product arr1 arr2) / ((sqrt (norm arr1)) * (sqrt (norm arr2)))

{--
    Computes the scalar product of 2 arrays.
--}
scalar_product :: [Float] -> [Float] -> Float
scalar_product [] [] = 0
scalar_product (x:xs) (y:ys) = (x * y) + (scalar_product xs ys)

{--
    Computes the norm^2 of an array.
--}
norm :: [Float] -> Float
norm arr = foldr (\x acc -> (acc + x * x)) 0 arr


{--
    Alternative to the above cosine function. It may be faster.

cosine :: [Float] -> [Float] -> Float
cosine arr1 arr2 = cosine_aux 0 0 0 arr1 arr2

cosine_aux :: Float -> Float -> Float -> [Float] -> [Float] -> Float
cosine_aux pr norm1 norm2 [] [] = pr / ((sqrt norm1) * (sqrt norm2))
cosine_aux pr norm1 norm2 (x:xs) (y:ys) =
    cosine_aux (pr + x * y) (norm1 + (x * x)) (norm2 + (y * y)) xs ys
--}