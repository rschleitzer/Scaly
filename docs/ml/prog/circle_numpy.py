# The circle learned with matrices, in Python with NumPy (Machine Learning
# with Scaly, chapter 1, "Why Without Python"): the network of circle.scaly
# on the same points from the same starting weights, written the way NumPy
# is written -- each layer one line, tanh(X W + b), each derivative an
# operation on whole matrices. The loops run inside NumPy, in C.

import numpy as np

seed = 88172645463325252
MASK = (1 << 64) - 1


def random():
    global seed
    x = seed
    x ^= (x << 13) & MASK
    x ^= x >> 7
    x ^= (x << 17) & MASK
    seed = x
    return (x >> 11) / 9007199254740992.0


def between(a, b):
    r = random()
    return a + (b - a) * r


# the starting weights as the field draws them (see circle.scaly)
W1, b1 = np.zeros((2, 4)), np.zeros((1, 4))
W2, b2 = np.zeros((4, 2)), np.zeros((1, 2))
W3, b3 = np.zeros((2, 1)), np.zeros((1, 1))
for place in range(7 * 72):
    r = between(0.0 - 0.5, 0.5)
    block, at = place // 72, place % 72
    i, m = at // 8, at % 8
    if at < 64:
        if block == 0 and i < 4 and m < 2:
            W1[m, i] = r
        if block == 1 and i < 2 and m < 4:
            W2[m, i] = r
        if block == 2 and i < 1 and m < 2:
            W3[m, i] = r
    else:
        if block == 0 and at - 64 < 4:
            b1[0, at - 64] = 0.1
        if block == 1 and at - 64 < 2:
            b2[0, at - 64] = 0.1
        if block == 2 and at - 64 < 1:
            b3[0, at - 64] = 0.1

# 400 points of the circle, the field's way
X, T = np.zeros((200, 2)), np.zeros((200, 1))
TX, TT = np.zeros((200, 2)), np.zeros((200, 1))
for p in range(400):
    kind = 1.0 if (p // 2) % 2 == 0 else 0.0 - 1.0
    r = between(0.0, 2.5) if kind > 0.0 else between(3.5, 5.0)
    angle = between(0.0, 6.283185307179586)
    between(0.0 - 5.0, 5.0)
    between(0.0 - 5.0, 5.0)
    if p % 2 == 0:
        X[p // 2] = (r * np.sin(angle), r * np.cos(angle))
        T[p // 2, 0] = kind
    else:
        TX[p // 2] = (r * np.sin(angle), r * np.cos(angle))
        TT[p // 2, 0] = kind


def answer(X):
    H1 = np.tanh(X @ W1 + b1)
    H2 = np.tanh(H1 @ W2 + b2)
    return np.tanh(H2 @ W3 + b3)


def loss(A, T):
    return float(np.mean(0.5 * (A - T) ** 2))


for s in range(501):
    if s % 100 == 0:
        print(f"step {s} train {loss(answer(X), T)!r} test {loss(answer(TX), TT)!r}")
    H1 = np.tanh(X @ W1 + b1)
    H2 = np.tanh(H1 @ W2 + b2)
    A = np.tanh(H2 @ W3 + b3)
    D3 = (A - T) * (1.0 - A * A)
    D2 = (D3 @ W3.T) * (1.0 - H2 * H2)
    D1 = (D2 @ W2.T) * (1.0 - H1 * H1)
    n = X.shape[0]
    W3 -= 0.03 * (H2.T @ D3 / n)
    b3 -= 0.03 * (D3.sum(axis=0, keepdims=True) / n)
    W2 -= 0.03 * (H1.T @ D2 / n)
    b2 -= 0.03 * (D2.sum(axis=0, keepdims=True) / n)
    W1 -= 0.03 * (X.T @ D1 / n)
    b1 -= 0.03 * (D1.sum(axis=0, keepdims=True) / n)
right = int(np.sum(answer(TX) * TT > 0.0))
print(f"test right {right} of 200")
