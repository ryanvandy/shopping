// Ensure Firebase modules are loaded before script.js runs
// They are exposed to window.firebase by the index.html script tag
const {
    initializeApp,
    getAuth,
    signInAnonymously,
    signInWithCustomToken,
    onAuthStateChanged,
    getFirestore,
    collection,
    addDoc,
    deleteDoc,
    onSnapshot,
    doc,
    query,
    where
} = window.firebase;

// Global variables provided by the Canvas environment
const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
const firebaseConfig = typeof __firebase_config !== 'undefined' ? JSON.parse(__firebase_config) : {};
const initialAuthToken = typeof __initial_auth_token !== 'undefined' ? __initial_auth_token : null;

let db;
let auth;
let userId = null;
let isAuthReady = false;
let targetAmazonList = [];
let aldiList = [];
let itemToDelete = null; // Stores the item object to be deleted

// Get DOM elements
const loadingOverlay = document.getElementById('loading-overlay');
const userIdDisplay = document.getElementById('user-id-display');
const userIdValue = document.getElementById('user-id-value');
const errorMessageDiv = document.getElementById('error-message');
const errorTextSpan = document.getElementById('error-text');
const groceryItemInput = document.getElementById('grocery-item-input');
const storeSelect = document.getElementById('store-select');
const addItemButton = document.getElementById('add-item-button');
const targetAmazonListUl = document.getElementById('target-amazon-list');
const aldiListUl = document.getElementById('aldi-list');
const targetAmazonEmptyMessage = document.getElementById('target-amazon-empty-message');
const aldiEmptyMessage = document.getElementById('aldi-empty-message');
const shareListsButton = document.getElementById('share-lists-button');
const confirmModal = document.getElementById('confirm-modal');
const itemToDeleteNameSpan = document.getElementById('item-to-delete-name');
const cancelDeleteButton = document.getElementById('cancel-delete-button');
const confirmDeleteButton = document.getElementById('confirm-delete-button');

// Function to display error messages
function showErrorMessage(message) {
    errorTextSpan.textContent = message;
    errorMessageDiv.classList.remove('hidden');
}

// Function to hide error messages
function hideErrorMessage() {
    errorMessageDiv.classList.add('hidden');
    errorTextSpan.textContent = '';
}

// Function to render a grocery list
function renderGroceryList(list, ulElement, emptyMessageElement) {
    ulElement.innerHTML = ''; // Clear current list items
    if (list.length === 0) {
        emptyMessageElement.classList.remove('hidden');
    } else {
        emptyMessageElement.classList.add('hidden');
        list.forEach(item => {
            const li = document.createElement('li');
            li.className = 'flex items-center justify-between bg-gray-50 p-4 rounded-md shadow-sm border border-gray-200';
            li.innerHTML = `
                <span class="text-lg text-gray-800 flex-grow">${item.name}</span>
                <button
                    data-id="${item.id}"
                    data-name="${item.name}"
                    class="ml-4 px-4 py-2 bg-red-500 text-white font-semibold rounded-md shadow-sm hover:bg-red-600 focus:outline-none focus:ring-2 focus:ring-red-500 focus:ring-offset-2 transition duration-200 ease-in-out delete-button"
                >
                    Delete
                </button>
            `;
            ulElement.appendChild(li);
        });
    }
}

// Initialize Firebase and set up authentication listener
async function initializeAppAndAuth() {
    try {
        const app = initializeApp(firebaseConfig);
        db = getFirestore(app);
        auth = getAuth(app);

        onAuthStateChanged(auth, async (user) => {
            if (user) {
                // User is signed in
                userId = user.uid;
                userIdValue.textContent = userId;
                userIdDisplay.classList.remove('hidden');
            } else {
                // User is signed out, sign in anonymously if no custom token
                try {
                    if (initialAuthToken) {
                        await signInWithCustomToken(auth, initialAuthToken);
                    } else {
                        await signInAnonymously(auth);
                    }
                } catch (error) {
                    console.error("Error signing in:", error);
                    showErrorMessage("Failed to authenticate. Please try again.");
                }
            }
            isAuthReady = true;
            loadingOverlay.classList.add('hidden'); // Hide loading overlay once auth is ready
            setupFirestoreListeners(); // Setup listeners after auth is ready
        });
    } catch (error) {
        console.error("Error initializing Firebase:", error);
        showErrorMessage("Failed to initialize Firebase. Check console for details.");
        loadingOverlay.classList.add('hidden'); // Hide loading overlay even on error
    }
}

// Setup Firestore listeners for real-time updates
function setupFirestoreListeners() {
    if (!db || !userId || !isAuthReady) {
        return;
    }

    const groceryCollectionRef = collection(db, `artifacts/${appId}/users/${userId}/groceries`);

    // Listener for Target/Amazon list
    const qTargetAmazon = query(groceryCollectionRef, where('store', '==', 'Target/Amazon'));
    onSnapshot(qTargetAmazon, (snapshot) => {
        targetAmazonList = snapshot.docs.map(doc => ({
            id: doc.id,
            ...doc.data()
        }));
        renderGroceryList(targetAmazonList, targetAmazonListUl, targetAmazonEmptyMessage);
    }, (error) => {
        console.error("Error fetching Target/Amazon grocery list:", error);
        showErrorMessage("Failed to load Target/Amazon list. Please refresh.");
    });

    // Listener for Aldi list
    const qAldi = query(groceryCollectionRef, where('store', '==', 'Aldi'));
    onSnapshot(qAldi, (snapshot) => {
        aldiList = snapshot.docs.map(doc => ({
            id: doc.id,
            ...doc.data()
        }));
        renderGroceryList(aldiList, aldiListUl, aldiEmptyMessage);
    }, (error) => {
        console.error("Error fetching Aldi grocery list:", error);
        showErrorMessage("Failed to load Aldi list. Please refresh.");
    });
}

// Function to add a new grocery item
async function addGroceryItem() {
    const itemText = groceryItemInput.value.trim();
    const selectedStore = storeSelect.value;

    if (!itemText) {
        showErrorMessage("Grocery item cannot be empty.");
        return;
    }
    if (!db || !userId) {
        showErrorMessage("Firebase not initialized or user not authenticated.");
        return;
    }

    try {
        hideErrorMessage(); // Clear previous error
        await addDoc(collection(db, `artifacts/${appId}/users/${userId}/groceries`), {
            name: itemText,
            store: selectedStore,
            timestamp: new Date(),
        });
        groceryItemInput.value = ''; // Clear input field
    } catch (e) {
        console.error("Error adding document: ", e);
        showErrorMessage("Failed to add item. Please try again.");
    }
}

// Function to confirm deletion
function confirmDeleteItem(id, name) {
    itemToDelete = { id, name };
    itemToDeleteNameSpan.textContent = name;
    confirmModal.classList.remove('hidden');
}

// Function to delete a grocery item
async function deleteGroceryItem() {
    if (!itemToDelete) return; // Should not happen if modal is shown correctly
    if (!db || !userId) {
        showErrorMessage("Firebase not initialized or user not authenticated.");
        return;
    }

    try {
        hideErrorMessage(); // Clear previous error
        await deleteDoc(doc(db, `artifacts/${appId}/users/${userId}/groceries`, itemToDelete.id));
        confirmModal.classList.add('hidden'); // Close modal after deletion
        itemToDelete = null; // Clear item to delete
    } catch (e) {
        console.error("Error deleting document: ", e);
        showErrorMessage("Failed to delete item. Please try again.");
    }
}

// Function to cancel deletion
function cancelDelete() {
    confirmModal.classList.add('hidden');
    itemToDelete = null;
}

// Function to handle sharing the lists via email
function shareListsByEmail() {
    let emailBody = "Here are my grocery lists:\n\n";

    if (targetAmazonList.length > 0) {
        emailBody += "--- Target/Amazon List ---\n";
        targetAmazonList.forEach(item => {
            emailBody += `- ${item.name}\n`;
        });
        emailBody += "\n";
    }

    if (aldiList.length > 0) {
        emailBody += "--- Aldi List ---\n";
        aldiList.forEach(item => {
            emailBody += `- ${item.name}\n`;
        });
        emailBody += "\n";
    }

    if (targetAmazonList.length === 0 && aldiList.length === 0) {
        emailBody += "Your grocery lists are currently empty.\n";
    }

    const subject = encodeURIComponent("My Grocery Lists");
    const body = encodeURIComponent(emailBody);

    window.location.href = `mailto:?subject=${subject}&body=${body}`;
}

// Event Listeners
addItemButton.addEventListener('click', addGroceryItem);
groceryItemInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        addGroceryItem();
    }
});

// Event delegation for delete buttons
document.addEventListener('click', (e) => {
    if (e.target && e.target.classList.contains('delete-button')) {
        const id = e.target.dataset.id;
        const name = e.target.dataset.name;
        confirmDeleteItem(id, name);
    }
});

cancelDeleteButton.addEventListener('click', cancelDelete);
confirmDeleteButton.addEventListener('click', deleteGroceryItem);
shareListsButton.addEventListener('click', shareListsByEmail);

// Initialize the application when the DOM is fully loaded
document.addEventListener('DOMContentLoaded', initializeAppAndAuth);