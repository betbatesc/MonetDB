try:
    from MonetDBtesting import process
except ImportError:
    import process
import os, sys
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
    tc.execute("select 'hello';")\
            .assertSucceeded()\
            .assertRowCount(1)

    sout, serr = slave.communicate()

##master = process.server(dbname = dbname, stdin = process.PIPE, stdout = process.PIPE, stderr = process.PIPE)
#with process.server(dbname=dbnameclone, mapiport='0', stdin=process.PIPE, stdout=process.PIPE, stderr=process.PIPE) as slave, \
#     process.client('sql', server = slave, stdin = process.PIPE, stdout = process.PIPE, stderr = process.PIPE) as c:
#
#    cout, cerr = c.communicate( f" select 'hello';")
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
    sys.stdout.write("#LISTING OF THE LOG FILES\n")
    for f in sorted(os.listdir(path)):
        if f.find('wlc') >= 0 and f != 'wlc_logs':
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
