
def Fat32_attrs_date(date_hex):
    date_hex = "0x"+date_hex
    date_bin = bin(eval(date_hex))
    #print date_bin
    tlen = len(date_bin)
    day = date_bin[tlen-5:tlen] #0bit - 4bit for day
    #print day
    month = date_bin[tlen-9:tlen-5] #5bit - 8bit for month
    #print month
    year = date_bin[2:tlen-9] #9bit - 15bit for year + 1980
    #print year
    print "year:",eval("0b"+year)+1980
    print "month:",eval("0b"+month)
    print "day:",eval("0b"+day)
    
def Fat32_attrs_time(time_hex):
    time_hex = "0x"+time_hex
    time_bin = bin(eval(time_hex))
    #print time_bin
    tlen = len(time_bin)
    sec = time_bin[tlen-5:tlen] #0bit - 4bit for second
    #print sec
    minute = time_bin[tlen-11:tlen-5] #5bit - 10bit for minute
    #print minute
    hour = time_bin[2:tlen-11]  #11bit - 15bit for hour
    #print hour
    print "sec:",eval("0b"+sec)*2
    print "min:",eval("0b"+minute)
    print "hour:",eval("0b"+hour)


if __name__ == '__main__':
    date = raw_input("input date:")
    Fat32_attrs_date(date)
    time = raw_input("input time:")
    Fat32_attrs_time(time)
