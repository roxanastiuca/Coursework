:- use_module(tables).
:- use_module(check_predicates).


plus5(X,Y):- Y is X + 5.

make_format_str(MaxRowLen,Str) :- maplist(plus5,MaxRowLen,Rp), 
									aux_format(Rp,Str).

aux_format([H],R) :- string_concat("~t~w~t~",H,R1),
						string_concat(R1,"+~n",R),!.
aux_format([H|T],R) :- string_concat("~t~w~t~",H,R1),
						string_concat(R1,"+ ",R2),
						aux_format(T,Rp),
						string_concat(R2,Rp,R).

% Predicate pentru aflarea head si tail
my_head([H | _], H).
my_tail([_ | T], T).


% Predicat care transpune o matrice
transpose([ [] | _], []).
transpose(T, [H | R]) :- transpose_column(T, H, Rest), transpose(Rest, R).

% Predicat care transpune o linie
transpose_column([], [], []).
transpose_column([ [H | T] | R], [H | Hs], [T | Ts]) :- transpose_column(R, Hs, Ts).


% Predicat care intoarce o lista cu lungimile cuvintelor dintr-o lista
get_l([], []).
get_l([H | T], [L | R]) :- string_length(H, L), get_l(T, R).

% Predicat care intoarce o lista cu lungimile maxime ale cuvintelor pentru fiecare coloana
get_length_list([], []).
get_length_list([H | T], [L | R]) :- get_l(H, List), max_list(List, L), get_length_list(T, R).

% Predicat care intoarce sirul de formatare pentru un tabel dat
get_format_str(Entries, Str) :- transpose(Entries, Trans), get_length_list(Trans, MaxRowLen), make_format_str(MaxRowLen, Str).

% Predicat auxiliar care printeaza un tabel cu un sir de formatare dat
print_table_aux(_, []).
print_table_aux(Str, [H | Entries]) :- format(Str, H), print_table_aux(Str, Entries). 

% Predicat care printeaza un tabel
print_table_op(Entries) :- get_format_str(Entries, Str), print_table_aux(Str, Entries).

% Predicat care realizeaza operatia Op pe intrarile a doua tabele
join_op_entries(_, [], [], []).
join_op_entries(Op, [H1 | T1], [H2 | T2], [H | R]) :- call(Op, H1, H2, H), join_op_entries(Op, T1, T2, R).

% Predicat care realizeaza operatia de join
join_op(Op, NewCols, [_ | T1], [_ | T2], [NewCols | R]) :- join_op_entries(Op, T1, T2, R).

% Predicat care realizeaza operatia de select
select_op(T, Cols, R) :- transpose(T, Tr), select_op_aux(Tr, Cols, RTr), transpose(RTr, R).

% Predicat auxiliar care realizeaza operatia de select pe un tabel transpus
select_op_aux([], _, []).
select_op_aux([H | T], Cols, [H | R]) :- my_head(H, First), member(First, Cols), select_op_aux(T, Cols, R).
select_op_aux([H | T], Cols, R) :- my_head(H, First), not(member(First, Cols)), select_op_aux(T, Cols, R).

% Filter 
csp(_, _, [], []).
csp(X, Cond, [H | T], R) :- not( (X = H, Cond) ), csp(X, Cond, T, R), !.
csp(X, Cond, [H | T], [H | R]) :- csp(X, Cond, T, R).

% Predicat care realizeaza filtrarea
filter_op([H | T], Vars, Pred, [H | R]) :- csp(Vars, Pred, T, R).

% Predicat care realizeaza evaluarea unui Query de orice tip
eval(table(Str), R) :- table_name(Str, R).
eval(tprint(Q), R) :- eval(Q, R), print_table_op(R).
eval(join(Pred, Cols, Q1, Q2), R) :- eval(Q1, R1), eval(Q2, R2), join_op(Pred, Cols, R1, R2, R).
eval(select(Columns, Q), R) :- eval(Q, Rp), select_op(Rp, Columns, R).
eval(tfilter(S, G, Q), R) :- eval(Q, Rp), filter_op(Rp, S, G, R).
eval(complex_query1(Q), R) :- eval(Q, Rp), complex_query1_op(Rp, R).
eval(complex_query2(GenreRef, Min, Max), R) :- eval(Q, Rp), complex_query2_op(GenreRef, Min, Max, Rp, R).

% Predicat care testeaza daca un sir este sufix pentru altul
suffix(Str, L2):- string_chars(Str, L1), append(_, L2, L1).

% Predicat care calculeaza media aritmetica a unei liste
average(List, Result) :- len(List, Len), sum(List, Sum), Result is Sum / Len.

% Predicat care calculeaza suma unei liste
sum([], 0).
sum([H | T], Sum) :- sum(T, Temp), Sum is Temp + H.

% Predicat care calculeaza lungimea unei liste
len([], 0).
len([_ | B], L) :- len(B, Ln), L is Ln + 1.

% Conditia pentru complex_query1
cond1(Last_name, AA, PP, PC, PA, POO) :- average([AA, PP], Med1), Med1 > 6, average([AA, PP, PC, PA, POO], Med2), Med2 > 5, suffix(Last_name, ['e', 's', 'c', 'u']).

% Predicat care realizeaza complex_query1 pe un tabel
complex_query1_op(Table, R) :- filter_op(Table, [_, Last_name, AA, PP, PC, PA, POO], cond1(Last_name, AA, PP, PC, PA, POO), R).

make_table([], []).
make_table([H1 | T], [H2 | R]) :- table_name(ratings, Table), 
									nth0(0, H1, IDt), 
									filter_op(Table, [_, _, IDr, _], (IDr == IDt), TableR), 
									nth0(1, TableR, Line), 
									nth0(3, Line, Rating), 
									append(H1, [Rating], H2), 
									make_table(T, R).

%cond2(GenreRef, Min, Max, Genre, Rating) :- split_string(Genre, ",", "", L), member(GenreRef, L), Min <= Rating, Rating <= Max.

complex_query2_op(GenreRef, Min, Max, [_ | _], R) :- 
									table_name(movies, [_ | T]), 
									make_table(T, Aux1), 
									append([ ["movie_id", "title", "genres", "rating"] ], Aux1, Aux2),
									filter_op(Aux2, [_, _, Genre, Rating], (split_string(Genre, ",", "", L), member(GenreRef, L), Min =< Rating, Rating =< Max), R).