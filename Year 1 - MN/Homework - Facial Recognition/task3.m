% This function computes the Ak matrix
% that is the result of applying svd
% on a given image, with a given k.

function [A_k S] = task3(image, k)
  A = double(imread(image));
  [m n] = size(A);
  
  % Find miu, the mean of each line and
  % extract it from A:
  miu = [];
  for i = 1 : m
    miu = [miu; mean(A(i,:))];
    A(i,:) = A(i,:) - miu(i,1);
  endfor
  
  Z = A'./sqrt(n - 1);
  
  % Find U, S and V for matrix Z:
  [U S V] = svd(Z);
  
  W = V(:, 1 : k);
  Y = W' * A;

  % Compute Ak based on mathematical formula:
  A_k = W * Y + miu;
endfunction