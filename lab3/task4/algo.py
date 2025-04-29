import numpy as np
from scipy.linalg import solve
import pytest

def affine_scaling_method(A, b, c, x0, alpha=0.5, tol=1e-6, max_iter=1000):
    """
    Параметры:

    A : ndarray, (M x N)

    b : ndarray, (M x 1)

    c : ndarray, (N x 1)

    x0 : ndarray, (N x 1)
        Начальная точка (Ax0 = b, x0 > 0).

    alpha : float
        Размер шага (0 < alpha < 1).

    tol : float
        Разница для остановки

    max_iter : int
        Максимальное число итераций.

    Возвращает:

    x : ndarray
        Решение.

    history : list
        История значений целевой функции.

    """
    x = x0.copy()
    history = []
    
    for _ in range(max_iter):
        D = np.diag(x)
        
        A_tilde = A @ D
        c_tilde = D @ c
        
        try:
            w = solve(A_tilde @ A_tilde.T, A_tilde @ c_tilde, assume_a='pos')
            r = c_tilde - A_tilde.T @ w 
        except np.linalg.LinAlgError:
            print("Error: A_tilde A_tilde^T is singular")
            break
        
        if np.all(r >= -tol):
            break
        
        p = -D @ r
        
        gamma = np.max(np.abs(p) / x)
        step = alpha / gamma
        
        x += step * p
        history.append(c @ x)
        
        if len(history) > 1 and abs(history[-1] - history[-2]) < tol:
            break
    
    return x, history

def test_case_1():
    A = np.array([[1, 1, 1], [2, 1, 0]])
    b = np.array([3, 2])
    c = np.array([-1, -2, 0])
    x0 = np.array([0.5, 1.0, 1.5])
    x_opt, _ = affine_scaling_method(A, b, c, x0, tol=1e-12)
    assert np.allclose(A @ x_opt, b, atol=1e-5)
    assert np.all(x_opt >= -1e-5)
    assert abs(c @ x_opt - (-4)) < 1e-5

def test_case_2():
    A = np.array([[1, 1, 1], [2, 2, 2]])
    b = np.array([3, 6])
    c = np.array([1, 1, 1])
    x0 = np.array([1, 1, 1])
    x_opt, _ = affine_scaling_method(A, b, c, x0, tol=1e-12)
    assert np.allclose(A @ x_opt, b, atol=1e-5)

def test_case_3():
    A = np.array([[1, -1, 1], [1, 1, 0]])
    b = np.array([2, 4])
    c = np.array([1, 1, 0])
    x0 = np.array([2.5, 1.5, 1])
    x_opt, _ = affine_scaling_method(A, b, c, x0, tol=1e-12, max_iter=5000)
    print(A @ x_opt, b)
    assert np.allclose(A @ x_opt, b, atol=1e-5)

if __name__ == "__main__":
    pytest.main([__file__, "-v"])