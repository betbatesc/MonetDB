try:
    from MonetDBtesting import process
except ImportError:
    import process
import os, sys, time
from MonetDBtesting.sqltest import SQLTestCase

dbfarm = os.getenv('GDK_DBFARM')
tstdb = os.getenv('TSTDB')

if not tstdb or not dbfarm:
    print('No TSTDB or GDK_DBFARM in environment')
    sys.exit(1)

dbname = tstdb
dbnameclone = tstdb + 'clone'

with process.server(dbname=dbnameclone, mapiport='0', stdin=process.PIPE, stdout=process.PIPE, stderr=process.PIPE) as slave, \
        SQLTestCase() as tc:
    tc.connect(database=dbnameclone, port=tc.dbport)
    tc.execute("select * from tmp;")\
            .assertSucceeded()\
            .assertDataResultMatch([(1, 'hello'), (2, 'world'), (3, 'blah'), (4, 'bloh'), (5, 'red'), (6, 'fox')])
    tc.execute("call wlr.master('%s');" % dbname).assertSucceeded()
    tc.execute("call wlr.replicate(-1);").assertFailed()
    tc.execute("call wlr.replicate(8);").assertSucceeded()
    tc.execute("select * from tmp;")\
            .assertSucceeded()\
            .assertDataResultMatch([(3, 'blah'), (2, 'blah'), (3, 'blah'), (4, 'blah'), (5, 'blah'), (6, 'blah')])

    sout, serr = slave.communicate()

##master = process.server(dbname = dbname, stdin = process.PIPE, stdout = process.PIPE, stderr = process.PIPE)
#with process.server(dbname=dbnameclone, mapiport='0', stdin=process.PIPE, stdout=process.PIPE, stderr=process.PIPE) as slave, \
#     process.client('sql', server = slave, stdin = process.PIPE, stdout = process.PIPE, stderr = process.PIPE) as c:
#
#    cout, cerr = c.communicate('''\
#select * from tmp;
#call wlr.master('%s');
#call wlr.replicate(-1);
#call wlr.replicate(8);
#select * from tmp;
#'''  % dbname)
#
#    sout, serr = slave.communicate()
#    #mout, merr = master.communicate()
#
#    #sys.stdout.write(mout)
#    sys.stdout.write(sout)
#    sys.stdout.write(cout)
#    #sys.stderr.write(merr)
#    sys.stderr.write(serr)
#    sys.stderr.write(cerr)

def listfiles(path):
    sys.stdout.write("#LISTING OF THE WLR LOG FILE\n")
    for f in sorted(os.listdir(path)):
        if f.find('wlr') >= 0:
            file = path + os.path.sep + f
            sys.stdout.write('#' + file + "\n")
            try:
                x = open(file)
                s = x.read()
                lines = s.split('\n')
                for l in lines:
                    sys.stdout.write('#' + l + '\n')
                x.close()
            except IOError:
                sys.stderr.write('Failure to read file ' + file + '\n')

# listfiles(os.path.join(dbfarm, tstdb))
# listfiles(os.path.join(dbfarm, tstdb, 'wlc_logs'))
listfiles(os.path.join(dbfarm, tstdb + 'clone'))
