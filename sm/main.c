#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "ui_glade.h"

#define SHOW_PASSWD 0
#define TERM  "/usr/bin/gnome-terminal"
#define SSH2  "/usr/local/bin/jnSSH2"

#define GET_WIDGET(name)    glade_xml_get_widget(glade_xml, name)

GladeXML *glade_xml;

struct row
{
    char ip[16];    
    char port[8];
    char user[16];
    char passwd[128];
};

// 用来绑定store中的数据
enum COL_NUMS {
    COL_IP,
    COL_PORT,
    COL_USER,
    COL_PASSWD,
};

void proc_child(int SIGNO)   
{   
    int status;
    wait(&status);
}   

void on_site_list_row_actived(GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *sel = gtk_tree_view_get_selection(view);
    gtk_tree_selection_get_selected(sel, &model, &iter);

    char *ip;
    char *port;
    char *user;
    char *passwd;
    gtk_tree_model_get(model, &iter, COL_IP, &ip, -1);
    gtk_tree_model_get(model, &iter, COL_PORT, &port, -1);
    gtk_tree_model_get(model, &iter, COL_USER, &user, -1);
    gtk_tree_model_get(model, &iter, COL_PASSWD, &passwd, -1);

    if (ip[0] == '\0' ||
        port[0] == '\0' ||
        user[0] == '\0' ||
        passwd[0] == '\0')
        return;

    pid_t pid = fork();

    if (pid < 0) {
        printf("fork error\n");
        return;
    }
    else if (pid == 0) {
        execl(TERM, TERM, "-x", SSH2, ip, port, user, passwd, NULL);
    }
}

void init_widget()
{
    GtkWidget *widget;
    GtkTreeView *view;
    GtkListStore *store;
    GtkCellRenderer *render;
    GtkTreeViewColumn *column;

    // view
    widget = GET_WIDGET("siteList");
    view = GTK_TREE_VIEW(widget);
    gtk_tree_view_set_grid_lines(view, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);

    // 文字渲染器
    render = gtk_cell_renderer_text_new();

    // 列
    // ip
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "IP");
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(column, 100);
    gtk_tree_view_column_pack_start (column, render, TRUE);
    gtk_tree_view_column_set_attributes (column, render, "text", COL_IP, NULL);
    gtk_tree_view_append_column(view, column);

    // port
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Port");
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(column, 50);
    gtk_tree_view_column_pack_start (column, render, TRUE);
    gtk_tree_view_column_set_attributes (column, render, "text", COL_PORT, NULL);
    gtk_tree_view_append_column(view, column);

    // user
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "User");
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(column, 100);
    gtk_tree_view_column_pack_start (column, render, TRUE);
    gtk_tree_view_column_set_attributes (column, render, "text", COL_USER, NULL);
    gtk_tree_view_append_column(view, column);

    // passwd
    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Passwd");
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width(column, 100);
    #if SHOW_PASSWD
    gtk_tree_view_column_pack_start (column, render, TRUE);
    gtk_tree_view_column_set_attributes (column, render, "text", COL_PASSWD, NULL);
    #endif
    gtk_tree_view_append_column(view, column);

    // 数据格式
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(view, GTK_TREE_MODEL(store));
    g_object_unref(store);

    g_signal_connect (view, "row-activated", G_CALLBACK (on_site_list_row_actived), NULL); 
}

void test()
{
    GtkWidget *widget;
    GtkTreeView *view;
    GtkListStore *store;
    GtkTreeIter iter;

    widget = GET_WIDGET("siteList");
    view = GTK_TREE_VIEW(widget);
    store = GTK_LIST_STORE(gtk_tree_view_get_model(view));

    gtk_list_store_clear(store);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, "10.1.44.212",
                       1, "22",
                       2, "root",
                       3, "zznode~!123",
                       -1);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, "10.1.100.156",
                       1, "22",
                       2, "root",
                       3, "zznode~!123",
                       -1);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, "10.1.12.50",
                       1, "22",
                       2, "root",
                       3, "zznode~!123",
                       -1);
}

void add_view(struct row *row)
{
    GtkWidget *widget;
    GtkTreeView *view;
    GtkListStore *store;
    GtkTreeIter iter;

    widget = GET_WIDGET("siteList");
    view = GTK_TREE_VIEW(widget);
    store = GTK_LIST_STORE(gtk_tree_view_get_model(view));

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, row->ip,
                       1, row->port,
                       2, row->user,
                       3, row->passwd,
                       -1);
}

int parse_line(struct row *row, char *line)
{
    char *p = memchr(line, '|', strlen(line));
    if (p == NULL)
        return -1;
    memcpy(row->ip, line, p-line);
    printf("%s\n", row->ip);
    line = p+1;

    p = memchr(line, '|', strlen(line));
    if (p == NULL)
        return -1;
    memcpy(row->port, line, p-line);
    printf("%s\n", row->port);
    line = p+1;

    p = memchr(line, '|', strlen(line));
    if (p == NULL)
        return -1;
    memcpy(row->user, line, p-line);
    printf("%s\n", row->user);
    line = p+1;

    p = memchr(line, '|', strlen(line));
    if (p == NULL)
        return -1;
    memcpy(row->passwd, line, p-line);
    printf("%s\n", row->passwd);
    line = p+1;

    return 0;
}

void load_list()
{
    char *home = getenv("HOME");
    printf("home: %s\n", home);

    char dir[256];
    memset(dir, 0x00, sizeof(dir));
    sprintf(dir, "%s/.jnSSH2", home);
    printf("dir: %s\n", dir);
    if (mkdir(dir, 0700) == -1) {
        printf("mkdir Error\n");
    }

    char filename[256];
    memset(filename, 0x00, sizeof(filename));
    sprintf(filename, "%s/account.lst", dir);

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s\n", line);

        struct row row;
        memset(&row, 0x00, sizeof(struct row));
        if (parse_line(&row, line) == 0)
            add_view(&row);
    }

    fclose(fp);
}

gboolean on_mainWindow_destroy(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
    gtk_main_quit();

    return TRUE;
}

int main(int argc, char *argv[])
{
    signal(SIGCHLD, proc_child);   


    gtk_init(&argc, &argv);

    /* load the interface */
    //glade_xml = glade_xml_new("ui.glade", NULL, NULL);
    glade_xml = glade_xml_new_from_buffer((char *)ui_glade, sizeof(ui_glade), NULL, NULL);

    /* connect the signals in the interface */
    glade_xml_signal_autoconnect(glade_xml);

    init_widget();

    load_list();

    /* start the event loop */
    gtk_main();

    return 0;
}
