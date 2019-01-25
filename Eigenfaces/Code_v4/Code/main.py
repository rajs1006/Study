import os
import numpy as np
import matplotlib as mpl
from PIL import Image

# Eigenfaces 
# .pgm or .png
def load_images(path: str, file_ending: str=".pgm") -> (list, int, int):

    images = []
    for file in sorted(os.listdir(path)):
        if file.endswith(file_ending):
            image = np.asarray(Image.open(path + file), dtype=np.float64)
            images.append(image)     
    
    image = images[0]
    dimension_y = image.shape[0]
    dimension_x = image.shape[1]
    
    return images, dimension_x, dimension_y


def setup_data_matrix(images: list) -> np.ndarray:
    
    D = np.zeros((len(images), images[0].shape[1] * images[0].shape[0]), dtype=np.float64)
    i = 0
    for i in range(len(images)):
        D[i] = images[i].flatten()

    return D


def calculate_pca(D: np.ndarray) -> (np.ndarray, np.ndarray, np.ndarray):
    
    mean_data = D.mean(0)
    
    D = D - mean_data
    u, svals, pcs = np.linalg.svd(D, full_matrices=False)
    
    return pcs, svals, mean_data

def accumulated_energy(singular_values: np.ndarray, threshold: float = 0.8) -> int:
    """
    Compute index k so that threshold percent of magnitude of singular values is contained in
    first k singular vectors.

    singular_values: vector containing singular values
    threshold: threshold for determining k (default = 0.8)
    """
    normalized = np.std(singular_values)
    i = 0
    for i in range(singular_values.shape[0]):
        singular_values[i] = singular_values[i] / normalized      
        
    acc_energy = 0
    k = 0
    while acc_energy <= threshold * np.sum(singular_values):
        acc_energy = acc_energy + singular_values[k]
        k = k + 1
    return k

def project_faces(pcs: np.ndarray, images: list, mean_data: np.ndarray) -> np.ndarray:

    coefficients = np.zeros((len(images), pcs.shape[0]))

    i = 0

    for i in range(coefficients.shape[0]):
        coefficients[i] = np.dot(pcs, images[i].flatten() - mean_data)
    
    return coefficients


def identify_faces(coeffs_train: np.ndarray, pcs: np.ndarray, mean_data: np.ndarray, path_test: str) -> (
np.ndarray, list, np.ndarray):

    imgs_test, dim_x, dim_y = load_images(path_test)
    
    coeffs_test = project_faces(pcs, imgs_test, mean_data)

    scores = np.zeros((coeffs_train.shape[0], coeffs_test.shape[0]))
    
    for i in range(coeffs_train.shape[0]):       
        for j in range(coeffs_test.shape[0]):
            scores[i][j] = np.arccos(np.dot(coeffs_train[i, :] / np.linalg.norm(coeffs_train[i, :]), coeffs_test[j, :] / np.linalg.norm(coeffs_test[j, :])))
                
    return scores, imgs_test, coeffs_test

