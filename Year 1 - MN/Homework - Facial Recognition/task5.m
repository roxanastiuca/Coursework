% This function creates 4 graphics about
% an image's singular values.

function task5(image, k)
  % Read image into matrix.
  A = double(imread(image));
  
  % Use own project's Ak computation function:
  [A_k S] = task3(image, k);
  
  [m n] = size(A);
  
  y2 = y3 = y4 = [];
  div = sum(diag(S)(1:min(m,n)));
  
  % Compute the values that are about to be plotted:
  for i = 1:k
    [A_i S] = task3(image, i);
    
    y2 = [y2, sum(diag(S)(1:i))/div];
    y3 = [y3, sum(sum((A - A_i).^2)) / (m*n)];
    y4 = [y4, (2*i + 1)/n];
  endfor
  
  subplot(2, 2, 1);
  plot([1 : min(m,n)], diag(S));
  title("Graphic 1 - diag(S) - Singular values");
  
  subplot(2, 2, 2);
  plot([1 : k], y2);
  title("Graphic 2 - Information given by the first k values");
  
  subplot(2, 2, 3);
  plot([1 : k], y3);
  title("Graphic 3 - The error of aproximation");
  
  subplot(2, 2, 4);
  plot([1 : k], y4);
  title("Grafic 4 - Data compression rate");
  
  hold on;
end