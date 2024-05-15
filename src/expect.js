spawn("bash -i")

stage("a")
    .expect("]# ").write("echo 'root'").goto("b")
    .expect("]$ ").write("echo 'user'").goto("b")

stage("b")
    .expect("]# ").write("ls -l | wc -l").next()

stage("c")
    .expect("]# ").write("exit").done()