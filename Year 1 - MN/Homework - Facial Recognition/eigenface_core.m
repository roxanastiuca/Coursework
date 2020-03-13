% This function is given dataset of 10 images and
% computes the average of them, their eigenvalues and
% eigenvectors and the images projection.

function [m A eigenfaces pr_img] = eigenface_core(database_path)
  T = [];
  
  % Read each image from dataset:
  for i = 1:10
    image_path = strcat(database_path,'/',int2str(i),'.jpg');
    matrix = double(rgb2gray(imread(image_path)));
    T = [T, matrix'(:)];
  end
  
  % Find mean of them:
  m = (mean(T'))';
  
  A = T - m;
  
  % Find eigenvalues and eigenvectors:
  [vectp valp] = eig(A' * A);
  [nl nc] = size(valp);

  V = [];
  
  for i = 1:nc
    if valp(i,i) > 1
      V = [V, vectp(:,i)];
    end
  end
  
  eigenfaces = A * V;
  
  % The images' projection:
  pr_img = eigenfaces' * A;
end