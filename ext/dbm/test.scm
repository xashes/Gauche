;;
;; test for dbm package
;;

(use gauche.test)
(use srfi-1)

(load "dbm")
(import dbm)

;;
;; Common test suite
;;

(define-syntax catch
  (syntax-rules ()
    ((_ body ...)
     (with-error-handler
      (lambda (e) #t)
      (lambda () body ... #f)))))

(define *test-dbm* "test.dbm")
(define *current-dbm* #f)

;; prepair dataset
(define *test1-dataset* (make-hash-table 'equal?)) ;string only
(define *test2-dataset* (make-hash-table 'equal?)) ;other objects

(define (limit-list list)
  (define limit 30)
  (if (> (length list) limit) (take list limit) list))

(let loop ((f (limit-list (sys-glob "/*"))))
  (cond ((null? f))
        ((and (file-is-directory? (car f))
              (sys-access (car f) |R_OK|))
         (for-each (lambda (s)
                     (hash-table-put! *test1-dataset*
                                      s (apply string-append s f))
                     (hash-table-put! *test2-dataset*
                                      (cons s f)
                                      (list (string-length s)
                                            (odd? (string-length s))
                                            (string-split s #\/))))
                   (limit-list (sys-glob (string-append (car f) "/*"))))
         (loop (cdr f)))
        (else (loop (cdr f)))))

;; create test
(define (test:make class rw-mode serializer)
  (set! *current-dbm*
        (dbm-open class
                  :path *test-dbm* :rw-mode rw-mode
                  :key-convert serializer
                  :value-convert serializer))
  #t)

;; put everything to the database
(define (test:put! dataset)
  (hash-table-for-each
   dataset
   (lambda (k v)
     (dbm-put! *current-dbm* k v)))
  #t)

;; does database has all of them?
(define (test:get dataset)
  (call/cc
   (lambda (return)
     (hash-table-for-each
      dataset
      (lambda (k v)
        (unless (dbm-exists? *current-dbm* k)
          (return #f))
        (unless (equal? v (dbm-get *current-dbm* k))
          (return #f))))
     #t)))

;; does database properly deal with exceptional case?
(define (test:get-exceptional)
  (and
   ;; must raise an error
   (catch (dbm-get *current-dbm* "this_is_not_a_key"))
   ;; use default
   (dbm-get *current-dbm* "this_is_not_a_key" #t)))

;; does for-each and map do a right thing?
(define (test:for-each dataset)
  (call/cc
   (lambda (return)
     (let ((r '()))
       (dbm-for-each *current-dbm*
                     (lambda (k v)
                       (unless (equal? v (hash-table-get dataset k #f))
                         (return #f))
                       (set! r (cons v r))))
       (equal? (reverse r)
               (dbm-map *current-dbm*
                        (lambda (k v) v)))))))

;; does delete work?
(define (test:delete dataset)
  (call/cc
   (lambda (return)
     (hash-table-for-each
      dataset
      (lambda (k v)
        (unless (and (dbm-exists? *current-dbm* k)
                     (begin (dbm-delete! *current-dbm* k)
                            (not (dbm-exists? *current-dbm* k))))
          (return #f))))
     #t)))

;; does read-only work?
(define (test:read-only)
  ;; if db is read-only, following procedures must throw an error.
  (and (catch (dbm-put! *current-dbm* "" ""))
       (catch (dbm-delete! *current-dbm* ""))))

;; does close work?
(define (test:close)
  (dbm-close *current-dbm*)
  (and (dbm-closed? *current-dbm*)
       ;; following procedures must throw an error.
       (catch (dbm-get *current-dbm* "" #f))
       (catch (dbm-exists? *current-dbm* ""))
       (catch (dbm-put! *current-dbm* "" ""))
       (catch (dbm-delete! *current-dbm* ""))
       (catch (dbm-for-each *current-dbm* (lambda _ #f)))
       (catch (dbm-map *current-dbm* (lambda _ #f)))))

;; clean up files
(define (clean-up)
  (when (file-exists? *test-dbm*) (sys-unlink *test-dbm*))
  (when (file-exists? (string-append *test-dbm* ".dir"))
    (sys-unlink (string-append *test-dbm* ".dir")))
  (when (file-exists? (string-append *test-dbm* ".pag"))
    (sys-unlink (string-append *test-dbm* ".pag"))))

;; a series of test per dataset and class
(define (run-through-test class dataset serializer)
  (define (tag msg) (format #f "~s ~a" class msg))
  (dynamic-wind
   clean-up
   (lambda ()
     ;; 1. create read/write db
     (test (tag "make") #t (lambda () (test:make class :create serializer)))
     ;; 2. put stuffs
     (test (tag "put!") #t (lambda () (test:put! dataset)))
     ;; 3, 4. get stuffs
     (test (tag "get") #t (lambda () (test:get dataset)))
     (test (tag "get-exceptional") #t (lambda () (test:get-exceptional)))
     ;; 5. for each
     (test (tag "for-each") #t (lambda () (test:for-each dataset)))
     ;; 6. close
     (test (tag "close") #t (lambda () (test:close)))
     ;; 7. open again with read only
     (test (tag "read-only open") #t (lambda () (test:make class :read serializer)))
     ;; 8. does it still have stuffs?
     (test (tag "get again") #t (lambda () (test:get dataset)))
     ;; 9. does it work as read-only?
     (test (tag "read-only") #t (lambda () (test:read-only)))
     ;; 10. close and open it again
     (test (tag "close again") #t
           (lambda ()
             (dbm-close *current-dbm*)
             (test:make class :write serializer)))
     ;; 11. delete stuffs
     (test (tag "delete") #t (lambda () (test:delete dataset)))
     ;; 12. close again
     (test (tag "close again") #t (lambda () (test:close))))
   clean-up))


;; Do test for two datasets
(define (full-test class)
  (test-section (format #f "~a dataset 1" (class-name class)))
  (run-through-test class *test1-dataset* #f)
  (test-section (format #f "~a dataset 2" (class-name class)))
  (run-through-test class *test2-dataset* #t)
  )

;; conditionally test
(define-macro (test-if-exists file module class)
  (if (file-exists? (string-append file ".so"))
      `(begin (require ,file) (import ,module) (full-test ,class))
      #f))

;;
;; GDBM test
;;

(test-if-exists "gdbm" dbm.gdbm <gdbm>)

;;
;; NDBM test
;;

(test-if-exists "ndbm" dbm.ndbm <ndbm>)

;;
;; DBM test
;;

(test-if-exists "odbm" dbm.odbm <odbm>)

(test-end)
