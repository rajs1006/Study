
import numpy as np
import matplotlib.pyplot as plt
import pylab

def visualize_eigenfaces(n: int, pcs: np.ndarray, sv: np.ndarray, dim_x: int, dim_y: int):
    
    fig = pylab.figure(figsize=(15, 8))
    m = int(np.ceil(n / 2))
    n = 2 * m

    for i in range(n):
        fig.add_subplot(2, m, i + 1)
        eface = pcs[i, :].reshape((dim_y, dim_x))
        plt.imshow(eface, cmap="Greys_r")
        plt.title('sigma = %.f' % sv[i])

    plt.show()


def plot_identified_faces(scores: np.ndarray, training_images: list, test_images: list, pcs: np.ndarray, coeffs_test: np.ndarray, mean_data: np.ndarray, threshold : float=0.8):
    # find best match, compute confidence and plot
    
    for i in range(scores.shape[1]):
        j = np.argmin(scores[:, i]) 
        fig = pylab.figure()
        
        fig.add_subplot(1, 3, 1)
        plt.imshow(test_images[i], cmap="Greys_r") 
        
        x = (test_images[i].flatten()-mean_data)[: coeffs_test.shape[1]];
        y = coeffs_test[i];
        face_space_distance = np.arccos(np.dot(x/np.linalg.norm(x),y/np.linalg.norm(y)));
        #print (face_space_distance)
    
        face_space = face_space_distance < (1 + threshold);
        face_class = np.where(scores[:, i] <= threshold)[0].size != 0
    
        if not face_space :
            plt.xlabel('no face identified')
        elif not face_class:
            plt.xlabel('face not matched')
        else:
            fig.add_subplot(1, 3, 2)
            plt.imshow(training_images[j], cmap="Greys_r")
            plt.xlabel('Identified person')
    
            fig.add_subplot(1, 3, 1)
            plt.imshow(test_images[i], cmap="Greys_r")
            plt.xlabel('Query image')
    
            img_reconst = pcs.transpose().dot(coeffs_test[i, :]) + mean_data
            img_reconst = img_reconst.reshape(test_images[i].shape)
    
            fig.add_subplot(1, 3, 3)
            plt.imshow(img_reconst, cmap="Greys_r")
            plt.xlabel('Reconstructed image')
    
            plt.show()
