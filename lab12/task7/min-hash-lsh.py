import re
import hashlib
from random import randint, seed
import math

   
class Shingler:
    def __init__(self, k=10):
        self.k = int(k) if k > 0 else 10
    
    def process_doc(self, document):
        """Очистка документа и приведение к нижнему регистру"""
        return re.sub(r"( )+|(\n)+", " ", document).lower()
    
    def get_shingles(self, document):
        """Генерация k-шинглов"""
        shingles = set()
        processed_doc = self.process_doc(document)
        for i in range(len(processed_doc) - self.k + 1):
            shingles.add(processed_doc[i:i+self.k])
        return shingles
    
    def get_hashed_shingles(self, shingles_set):
        """Хеширование шинглов для экономии памяти"""
        return {hash(shingle) for shingle in shingles_set}

class HashFamily:
    def __init__(self, i):
        self.result_size = 8  # Количество байт в хеше
        self.max_len = 20    # Максимальная длина соли
        self.salt = str(i).zfill(self.max_len)[-self.max_len:]
    
    def get_hash_value(self, el_to_hash):
        """Генерация хеш-значения с солью"""
        return int(hashlib.sha1(str(el_to_hash).encode('utf-8') + self.salt.encode('utf-8')).hexdigest()[:self.result_size], 16)

class MinHashSigner:
    def __init__(self, sig_size=100):
        self.sig_size = sig_size
        self.hash_functions = [HashFamily(randint(0, 10000000000)) for _ in range(sig_size)]
    
    def compute_set_signature(self, set_):
        """Вычисление MinHash сигнатуры для множества"""
        set_sig = []
        for h_funct in self.hash_functions:
            min_hash = math.inf
            for el in set_:
                h = h_funct.get_hash_value(el)
                if h < min_hash:
                    min_hash = h
            set_sig.append(min_hash)
        return set_sig
    
    def compute_signature_matrix(self, set_list):
        """Создание матрицы сигнатур для всех документов"""
        return [self.compute_set_signature(s) for s in set_list]

class LSH:
    def __init__(self, threshold=0.8):
        self.threshold = threshold
    
    def get_signature_matrix_bands(self, sig_matrix, bands_nr, sign_len): 
        #bands_nr = b
        #sign_len = n
        r = int(sign_len/bands_nr) #number of rows in each band
        bands = {} # {band_nr: [col_1,col_2,...]} where col_1 is all the values of Sig(S_i) for band b.
        for i in range(0,bands_nr):
            bands[i] = []
        # put Subsets of the columns of signature matrix into the appropriate bucket and cosider a column 
        # as a unique block so that we can hash the entire column.
        # Basically a band is a list of element, where each element is a subset of a signature of a given set.
        for signature in sig_matrix: 
            for i in range(0, bands_nr):
                idx = i*r    
                bands[i].append(' '.join(str(x) for x in signature[idx:idx+r]) )          
        return bands
    
    def get_band_buckets(self, band, hash_funct):
        """Создание buckets для band"""
        buckets = {}
        for doc_id in range(0,len(band)):
            value = hash_funct.get_hash_value( band[doc_id] )
            if value not in buckets:
                buckets[value] = [doc_id]
            else:
                buckets[value].append(doc_id)  

        return buckets
    
    def get_candidates_list(self, buckets):
        candidates = set()
        # buckets is a dictionary containing key=bucket, value= list of doc_ids that hashed to bucket
        for bucket,candidate_list in buckets.items():
            if len(candidate_list) > 1:
                for i in range(0,len(candidate_list)-1):
                    for j in range(i+1,len(candidate_list)):  
                        pair = tuple(sorted( (candidate_list[i],candidate_list[j]) ))
                        candidates.add(pair)
                
        return candidates #ie a set of couples, each couple is a candidate pair
    
    def check_candidates(self, candidates_list, threshold, sigs):
        similar_docs = set() #set of tuples
        # similar_pair is a couple containing doc_ids of documents that hashed to same bucket
        for  similar_pair in candidates_list:
            #for all the pairs of document in the list check similarity of their signatures
            doc_id_1 = similar_pair[0]
            doc_id_2 = similar_pair[1]
            signature_1 = set(sigs[doc_id_1]) #get the i-th column from signature matrix where i is doc_id in the collision list
            signature_2 = set(sigs[doc_id_2])
            js = len(signature_1.intersection(signature_2)) /len(signature_1.union(signature_2))
            if js >= threshold:
                similar_docs.add( tuple(sorted((doc_id_1,doc_id_2) )) )          
        return similar_docs
    
    def get_similar_items(self, sig_matrix, bands_nr, sign_len):
        similar_docs = set()
        #divide signature matrix into bands
        bands = lsh_instance.get_signature_matrix_bands(sig_matrix,bands_nr,sign_len)
        #for all the bands
        for band_id, elements in bands.items():
            #produce the buckets for the given band (band_id) with a random hash function
            buckets = lsh_instance.get_band_buckets(elements, hash_funct=HashFamily(randint(0,10000000000)))
            #Get all the candidate pairs
            candidates = lsh_instance.get_candidates_list(buckets)
            #Check all candidate pairs' signatures
            for sim_tuple in lsh_instance.check_candidates(candidates, self.threshold, sig_matrix):
                similar_docs.add( sim_tuple)
        return similar_docs #return all the similar signatures that respect the threshold

if __name__ == "__main__":
    shingler_inst = Shingler(10)
    with open("text.txt", 'r') as file:
        text = file.read()

    sentences = re.split(r'(?<!\w\.\w.)(?<![A-ZА-Я][a-zа-я]\.)(?<=\.|\?|\!)\s', text)
    sentences = [s.strip() for s in sentences if s.strip()]

    shinglings = []

    for doc in sentences:
        curr_shingling = shingler_inst.get_shingles(doc)
        # curr_shingling = shingler_inst.get_hashed_shingles(curr_shingling)
        shinglings.append(curr_shingling)

    signer = MinHashSigner(100)
    signature_matrix = signer.compute_signature_matrix(shinglings)
    lsh_instance = LSH(threshold=0.7)
    similarities = lsh_instance.get_similar_items(signature_matrix,10,100)
    for elem in similarities:
        first, second = elem
        print(sentences[first])
        print(sentences[second])
        print(str("-" * 20))