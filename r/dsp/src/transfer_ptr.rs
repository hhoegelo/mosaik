use std::{ptr, sync::atomic::AtomicPtr, sync::atomic::Ordering};

pub struct TransferPointer<T> {
    next: AtomicPtr<T>,
    current: *mut T,
    doomed: AtomicPtr<T>,
}

impl<T> TransferPointer<T> {
    pub fn new(p: *mut T) -> Self {
        Self {
            next: AtomicPtr::from(ptr::null_mut()),
            current: p,
            doomed: AtomicPtr::from(ptr::null_mut()),
        }
    }

    pub fn set(&mut self, p: *mut T) {
        let doomed = self.doomed.swap(ptr::null_mut(), Ordering::Relaxed);

        if doomed != ptr::null_mut() {
            unsafe {
                std::ptr::drop_in_place(doomed);
            }
        }

        let old_next = self.next.swap(p, Ordering::Relaxed);

        if old_next != ptr::null_mut() {
            unsafe {
                std::ptr::drop_in_place(old_next);
            };
        }
    }

    pub fn get(&mut self) -> *mut T {
        if self.doomed.as_ptr() != ptr::null_mut() {
            let n = self.next.swap(ptr::null_mut(), Ordering::Relaxed);
            if n != ptr::null_mut() {
                self.doomed.store(self.current, Ordering::Relaxed);
                self.current = n;
            }
        }
        return self.current;
    }
}

#[cfg(test)]
mod tests {
    use std::sync::{Arc, Mutex};

    use super::*;

    struct TestObject {
        drop_called: Arc<Mutex<bool>>,
    }

    impl Drop for TestObject {
        fn drop(&mut self) {
            *self.drop_called.lock().unwrap() = true;
        }
    }

    #[test]
    fn drop_is_called() {
        let dropon_initial_object_called = Arc::new(Mutex::new(false));
        let initial_object = Box::into_raw(Box::new(TestObject {
            drop_called: dropon_initial_object_called.clone(),
        }));
        let mut ptr = TransferPointer::<TestObject>::new(initial_object);

        let dropon_next_object_called = Arc::new(Mutex::new(false));
        let next_object = Box::into_raw(Box::new(TestObject {
            drop_called: dropon_next_object_called.clone(),
        }));

        assert!(ptr.get() == initial_object);
        assert!(!(*dropon_initial_object_called.lock().unwrap()));
        assert!(!(*dropon_next_object_called.lock().unwrap()));

        ptr.set(next_object);

        assert!(ptr.get() == next_object);
        assert!(!(*dropon_initial_object_called.lock().unwrap()));
        assert!(!(*dropon_next_object_called.lock().unwrap()));

        let dropon_verynext_object_called = Arc::new(Mutex::new(false));
        let verynext_object = Box::into_raw(Box::new(TestObject {
            drop_called: dropon_verynext_object_called.clone(),
        }));

        ptr.set(verynext_object);

        assert!(ptr.get() == verynext_object);
        assert!((*dropon_initial_object_called.lock().unwrap()));
        assert!(!(*dropon_next_object_called.lock().unwrap()));

    }
}
