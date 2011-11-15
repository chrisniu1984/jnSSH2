#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
import gtk
import signal
from multiprocessing import Process

XTERMS = (("/usr/bin/gnome-terminal", "-x"),
          ("/usr/bin/xfce4-terminal", "-x"),
          ("/usr/bin/xterm", "-e"))

class Config:
    def __init__(self):
        self.lists = []

    def read(self, path):
        fh = open(path, 'r')
        if fh == None:
            return

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
        os.execl(self.term, self.term, self.param, self.ssh2, ip, port, user, passwd)

    def do_row_actived(self, treeview, row=0, column=None, data=None):
        model = treeview.get_model()
        iter = model.get_iter(row)
        ip = model.get(iter, 0)[0]
        port =  model.get(iter, 1)[0]
        user = model.get(iter, 2)[0]
        passwd = model.get(iter, 3)[0]
        p = Process(target=self.run_it, args=(ip,port,user,passwd))
        p.start()
        gtk.main_quit();

    def do_destroy(self, widget, data=None):
        gtk.main_quit()

    def __term__(self):
        global XTERMS

        for m in XTERMS:
            print m[0]
            if os.access(m[0], os.X_OK|os.R_OK) == True:
                self.term=m[0];
                self.param=m[1];
                return

    def __init__(self):
        self.__term__()
        self.ssh2 = "/usr/local/bin/jnSSH2"
        signal.signal(signal.SIGCHLD, signal.SIG_IGN)

        # create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.set_title("jnSSH2 Launcher")
        self.window.set_position(gtk.WIN_POS_CENTER)
        self.window.set_border_width(3)
        self.window.set_resizable(False)
        self.window.connect("destroy", self.do_destroy)

        # List
        self.treeview = gtk.TreeView(model=None)
        self.treeview.set_grid_lines(gtk.TREE_VIEW_GRID_LINES_HORIZONTAL)

        render = gtk.CellRendererText()

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

    def bind(self,config):
        if config == None:
            return

        for line in config.lists:
            self.liststore.append([line["ip"],line["port"],line["user"],line["passwd"],line["comment"]])

    def main(self):
        gtk.main()

if __name__ == "__main__":
    fname = "%s/.jnSSH2/account.lst" % (os.environ['HOME'])
    config = Config()
    config.read(fname)

    window = Window()
    window.bind(config)
    window.main()
