% This function is similar to task3,
% it just doesn't use svd function anymore,
% and it computes Ak using eigenvalues and
% eigenvectors.

function [A_k S] = task4(image, k)
   A = double(imread(image));
   [m n] = size(A);
   
   % Find miu, the mean of each line and
   % substract it from A:
   miu = [];
   for i = 1 : m
     miu = [miu; mean(A(i,:))];
     A(i,:) = A(i,:) - miu(i,1);
   endfor
   
   Z = 1/(n-1) * A * A';
   
   % Find eigenvalues and eigenvectors:
   [V S] = eig(Z);

   W = V(:, 1:k);
   Y = W' * A;
   
   % Compute Ak based on mathematical formula:
   A_k = W * Y + miu;
endfunction