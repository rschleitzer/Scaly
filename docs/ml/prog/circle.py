# The circle learned with matrices, in Python as it comes (Machine Learning
# with Scaly, chapter 1, "Why Without Python"): circle.scaly line for line --
# the same random numbers, points and starting weights, the same network and
# the same sums in the same order --, every number a Python float and every
# loop a Python loop.

import math

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


class Matrix:
    def __init__(self, rows, cols):
        self.rows = rows
        self.cols = cols
        self.data = [0.0] * (rows * cols)

    def at(self, i, j):
        return self.data[i * self.cols + j]

    def put(self, i, j, v):
        self.data[i * self.cols + j] = v


def layer(X, W, b):
    H = Matrix(X.rows, W.cols)
    for i in range(X.rows):
        for j in range(H.cols):
            z = b.at(0, j)
            for k in range(X.cols):
                z = z + W.at(k, j) * X.at(i, k)
            H.put(i, j, math.tanh(z))
    return H


def loss(A, T):
    total = 0.0
    for i in range(A.rows):
        e = A.at(i, 0) - T.at(i, 0)
        total = total + 0.5 * e * e
    return total / A.rows


def out_share(A, T):
    D = Matrix(A.rows, A.cols)
    for i in range(D.rows):
        for j in range(D.cols):
            o = A.at(i, j)
            D.put(i, j, (o - T.at(i, j)) * (1.0 - o * o))
    return D


def back(D, W, H):
    below = Matrix(D.rows, W.rows)
    for i in range(D.rows):
        for k in range(below.cols):
            s = 0.0
            for j in range(W.cols):
                s = s + D.at(i, j) * W.at(k, j)
            out = H.at(i, k)
            below.put(i, k, s * (1.0 - out * out))
    return below


def weight_grad(H, D):
    G = Matrix(H.cols, D.cols)
    for k in range(H.cols):
        for j in range(G.cols):
            s = 0.0
            for i in range(D.rows):
                s = s + D.at(i, j) * H.at(i, k)
            G.put(k, j, s)
    return G


def bias_grad(D):
    G = Matrix(1, D.cols)
    for j in range(D.cols):
        s = 0.0
        for i in range(D.rows):
            s = s + D.at(i, j)
        G.put(0, j, s)
    return G


def descend(W, G, rate, points):
    for k in range(W.rows * W.cols):
        W.data[k] = W.data[k] - rate * (G.data[k] / points)


class Net:
    def __init__(self):
        self.W1, self.b1 = Matrix(2, 4), Matrix(1, 4)
        self.W2, self.b2 = Matrix(4, 2), Matrix(1, 2)
        self.W3, self.b3 = Matrix(2, 1), Matrix(1, 1)


def answer(net, X):
    return layer(layer(layer(X, net.W1, net.b1), net.W2, net.b2), net.W3, net.b3)


def step(net, X, T, rate):
    H1 = layer(X, net.W1, net.b1)
    H2 = layer(H1, net.W2, net.b2)
    A = layer(H2, net.W3, net.b3)
    D3 = out_share(A, T)
    D2 = back(D3, net.W3, H2)
    D1 = back(D2, net.W2, H1)
    descend(net.W3, weight_grad(H2, D3), rate, X.rows)
    descend(net.b3, bias_grad(D3), rate, X.rows)
    descend(net.W2, weight_grad(H1, D2), rate, X.rows)
    descend(net.b2, bias_grad(D2), rate, X.rows)
    descend(net.W1, weight_grad(X, D1), rate, X.rows)
    descend(net.b1, bias_grad(D1), rate, X.rows)


def report(s, net, X, T, TX, TT):
    train = loss(answer(net, X), T)
    test = loss(answer(net, TX), TT)
    print(f"step {s} train {train!r} test {test!r}")


# the starting weights as the field draws them (see circle.scaly)
net = Net()
for place in range(7 * 72):
    r = between(0.0 - 0.5, 0.5)
    block, at = place // 72, place % 72
    i, m = at // 8, at % 8
    if at < 64:
        if block == 0 and i < 4 and m < 2:
            net.W1.put(m, i, r)
        if block == 1 and i < 2 and m < 4:
            net.W2.put(m, i, r)
        if block == 2 and i < 1 and m < 2:
            net.W3.put(m, i, r)
    else:
        if block == 0 and at - 64 < 4:
            net.b1.put(0, at - 64, 0.1)
        if block == 1 and at - 64 < 2:
            net.b2.put(0, at - 64, 0.1)
        if block == 2 and at - 64 < 1:
            net.b3.put(0, at - 64, 0.1)

# 400 points of the circle, the field's way
X, T = Matrix(200, 2), Matrix(200, 1)
TX, TT = Matrix(200, 2), Matrix(200, 1)
for p in range(400):
    kind = 1.0 if (p // 2) % 2 == 0 else 0.0 - 1.0
    r = between(0.0, 2.5) if kind > 0.0 else between(3.5, 5.0)
    angle = between(0.0, 6.283185307179586)
    between(0.0 - 5.0, 5.0)
    between(0.0 - 5.0, 5.0)
    if p % 2 == 0:
        X.put(p // 2, 0, r * math.sin(angle))
        X.put(p // 2, 1, r * math.cos(angle))
        T.put(p // 2, 0, kind)
    else:
        TX.put(p // 2, 0, r * math.sin(angle))
        TX.put(p // 2, 1, r * math.cos(angle))
        TT.put(p // 2, 0, kind)

for s in range(501):
    if s % 100 == 0:
        report(s, net, X, T, TX, TT)
    step(net, X, T, 0.03)
A = answer(net, TX)
right = sum(1 for p in range(200) if A.at(p, 0) * TT.at(p, 0) > 0.0)
print(f"test right {right} of 200")
