class SiginfoPrinter:
    def __init__(self, val):
        self.val = val

    
def car_pp_func(val):
    if str(val.type) == 'Car *' : return SiginfoPrinter(val)

gdb.pretty_printers.append(car_pp_func)