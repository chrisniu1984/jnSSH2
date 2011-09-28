#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import gtk
from multiprocessing import Process
import signal

class Config:
    def __init__(self):
        self.lists = []

    def read(self, path):
        fh = open(path, 'r')
        lines = fh.readlines()
        fh.close()
        for line in lines:
            #去除空白
            line = line.strip()

            #忽略注释
            if line.startswith("#") == True:
                continue

            cols = line.split("|");
            self.lists.append({"ip":cols[0], "port":cols[1], "user":cols[2], "passwd":cols[3], "comment":cols[4]});

    def show(self):
        for line in self.lists:
            print line["ip"]
            print line["port"]
            print line["user"]
            print line["passwd"]
            print line["comment"]
            print "------------------------"

class Window:
    def run_it(self, ip, port, user, passwd):
        print ip
        print port
        print user
        print passwd
        os.execl(self.term, self.term, "-x", self.ssh2, ip, port, user, passwd)

    def do_row_actived(self, treeview, row=0, column=None, data=None):
        model = treeview.get_model()
        iter = model.get_iter(row)
        ip = model.get(iter, 0)[0]
        port =  model.get(iter, 1)[0]
        user = model.get(iter, 2)[0]
        passwd = model.get(iter, 3)[0]
        p = Process(target=self.run_it, args=(ip,port,user,passwd))
        p.start()

    def do_destroy(self, widget, data=None):
        gtk.main_quit()

    def __init__(self):
        self.term = "/usr/bin/gnome-terminal"
        self.ssh2 = "/usr/local/bin/jnSSH2"
        signal.signal(signal.SIGCHLD, signal.SIG_IGN)

        # create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.do_destroy)

        # Sets the border width of the window.
        self.window.set_border_width(3)

        render = gtk.CellRendererText()

        self.treeview = gtk.TreeView(model=None)
        self.treeview.set_grid_lines(gtk.TREE_VIEW_GRID_LINES_HORIZONTAL)

        column = gtk.TreeViewColumn()
        column.set_title("IP")
        column.pack_start(render, True);
        column.add_attribute(render, "text", 0);
        self.treeview.append_column(column)

        column = gtk.TreeViewColumn()
        column.set_title("Port")
        column.pack_start(render, True);
        column.add_attribute(render, "text", 1);
        self.treeview.append_column(column)

        column = gtk.TreeViewColumn()
        column.set_title("User")
        column.pack_start(render, True);
        column.add_attribute(render, "text", 2);
        self.treeview.append_column(column)

        column = gtk.TreeViewColumn()
        column.set_title("Passwd")
        column.pack_start(render, True);
        column.add_attribute(render, "text", 3);
        self.treeview.append_column(column)

        column = gtk.TreeViewColumn()
        column.set_title("Comment")
        column.pack_start(render, True);
        column.add_attribute(render, "text", 4);
        self.treeview.append_column(column)

        self.liststore = gtk.ListStore(str, str, str, str, str)
        self.treeview.set_model(self.liststore)

        self.window.add(self.treeview)
        self.treeview.show()

        self.treeview.connect ("row-activated", self.do_row_actived, None)

        self.window.show()

    def config(self,config):
        for line in config.lists:
            self.liststore.append([line["ip"],line["port"],line["user"],line["passwd"],line["comment"]])

    def main(self):
        gtk.main()

if __name__ == "__main__":
    config = Config()
    config.read('/home/niu/.jnSSH2/account.lst')

    window = Window()
    window.config(config)
    window.main()
