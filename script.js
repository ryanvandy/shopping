document.addEventListener('DOMContentLoaded', () => {
    // Load lists when the page loads
    loadList('aldi');
    loadList('target');
});

function addItem(listName) {
    const inputId = `${listName}-item-input`;
    const listId = `${listName}-list`;
    const itemInput = document.getElementById(inputId);
    const itemText = itemInput.value.trim();

    if (itemText !== "") {
        const ul = document.getElementById(listId);
        const li = document.createElement('li');
        li.innerHTML = `
            <span>${itemText}</span>
            <button class="delete-button" onclick="deleteItem(this, '${listName}')">Delete</button>
        `;
        ul.appendChild(li);

        saveList(listName); // Save the updated list to localStorage
        itemInput.value = ''; // Clear the input field
    }
}

function deleteItem(buttonElement, listName) {
    const li = buttonElement.parentNode;
    li.parentNode.removeChild(li); // Remove the list item from the DOM
    saveList(listName); // Save the updated list to localStorage
}

function saveList(listName) {
    const listId = `${listName}-list`;
    const ul = document.getElementById(listId);
    const items = [];
    ul.querySelectorAll('li span').forEach(span => {
        items.push(span.textContent);
    });
    // Store the array of items as a JSON string in localStorage
    localStorage.setItem(`${listName}Items`, JSON.stringify(items));
}

function loadList(listName) {
    const listId = `${listName}-list`;
    const ul = document.getElementById(listId);
    // Retrieve the JSON string from localStorage and parse it back to an array
    const storedItems = JSON.parse(localStorage.getItem(`${listName}Items`));

    if (storedItems) {
        storedItems.forEach(itemText => {
            const li = document.createElement('li');
            li.innerHTML = `
                <span>${itemText}</span>
                <button class="delete-button" onclick="deleteItem(this, '${listName}')">Delete</button>
            `;
            ul.appendChild(li);
        });
    }
}