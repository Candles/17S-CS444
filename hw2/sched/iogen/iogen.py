from random import shuffle

files = [open(str(i*10000+x)+".txt", "w+") for x in range(1000)]

shuffle(files)

for index, f in enumerate(files):
    f.write(str(index) * index)

shuffle(files)

for f in files:
    f.close()
