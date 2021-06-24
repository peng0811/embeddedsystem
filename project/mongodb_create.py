import pymongo as pym      
import pandas as pd        
import os                  
import csv                 
import json                

def xlsx_to_csv_pd():
    data_xls = pd.read_excel('data3.xlsx', index_col=0)
    data_xls.to_csv('data3.csv', encoding='utf-8')
if __name__ == '__main__':
    xlsx_to_csv_pd()

def os_any_dir_search(file):
    u=[]
    for p,n,f in os.walk(os.getcwd()):
        
        for a in f:
            a = str(a)
            if a.endswith(file):
                t=pd.read_csv(p+'/'+file)
                u.append(p+'/'+a)
    return t,u


data = os_any_dir_search('data3.csv')[0]
#fake_users=fake_users.iloc[:,1:]
#print(fake_users.head())

data.to_csv('data3_fixed.csv')
data = data.to_dict('records')
#print(fake_users_dict[:5])

client = pym.MongoClient('mongodb://localhost:27017/')
db = client["measurement"]
user_info_table= db["temp_taipei"]
user_info_table.insert_many(data)
