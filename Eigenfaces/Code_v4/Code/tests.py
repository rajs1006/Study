
import numpy as np
import matplotlib.pyplot as plt
import unittest
from main import load_images, setup_data_matrix, accumulated_energy,calculate_pca, project_faces, \
    identify_faces
from lib import visualize_eigenfaces, plot_identified_faces


class Tests(unittest.TestCase):

    def setup_tests(self, stage, cutoff_threshold=0.8):
        # Read training data set
        self.imgs_train, self.dim_x, self.dim_y = load_images("./data/train/")
        if stage == "load_images":
            return

        # compute data matrix
        self.D = setup_data_matrix(self.imgs_train)
        if stage == "setup_data_matrix":
            return

        # Perform principal component analysis
        self.pcs, self.sv, self.mean_data = calculate_pca(self.D)
        if stage == "calculate_pca":
            return
        
        # compute threshold for 90% of spectral energy
        self.k = accumulated_energy(self.sv, cutoff_threshold)
        if stage == "accumulated_energy":
            return

        # cut off number of pcs if desired
        self.pcs = self.pcs[0:self.k, :]
        # compute coefficients of input in eigenbasis
        self.coeffs_train = project_faces(self.pcs, self.imgs_train, self.mean_data)
        if stage == "project_faces":
            return

        # perform classical face recognition
        self.scores, self.imgs_test, self.coeffs_test = identify_faces(self.coeffs_train, self.pcs, self.mean_data,
                                                                       './data/test/')


    def test_calculate_pca(self):
        self.setup_tests("calculate_pca")
        # Visualize the eigenfaces/principal components
        visualize_eigenfaces(10, self.pcs, self.sv, self.dim_x, self.dim_y)

    def test_identify_faces(self):
        self.setup_tests("identify_faces")
        plot_identified_faces(self.scores, self.imgs_train, self.imgs_test, self.pcs, self.coeffs_test, self.mean_data)


if __name__ == '__main__':
    unittest.main()

